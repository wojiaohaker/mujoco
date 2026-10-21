/**
 * MujocoSim 实现
 *
 * 关节顺序 (MuJoCo qpos[7:18], 与 RobotState protobuf 对应):
 *   [0] FAR_ABAD  (FR外展)  → q_abad[0]
 *   [1] FAR_HIP   (FR髋)    → q_hip[0]
 *   [2] FAR_KNEE  (FR膝)    → q_knee[0]
 *   [3] FBL_ABAD  (FL外展)  → q_abad[1]
 *   [4] FBL_HIP   (FL髋)    → q_hip[1]
 *   [5] FBL_KNEE  (FL膝)    → q_knee[1]
 *   [6] RAR_ABAD  (RR外展)  → q_abad[2]
 *   [7] RAR_HIP   (RR髋)    → q_hip[2]
 *   [8] RAR_KNEE  (RR膝)    → q_knee[2]
 *   [9] RBL_ABAD  (RL外展)  → q_abad[3]
 *  [10] RBL_HIP   (RL髋)    → q_hip[3]
 *  [11] RBL_KNEE  (RL膝)    → q_knee[3]
 *
 * 传感器索引 (xgb.xml <sensor> 定义顺序):
 *   [0:11]  jointpos  (FR,FL,RR,RL × abad,hip,knee)
 *   [12:23] jointvel
 *   [24:35] jointactuatorfrc (torque)
 *   [36:39] framequat (imu_quat: w,x,y,z)
 *   [39:41] gyro (imu_gyro)
 *   [42:44] accelerometer (imu_acc)
 *   [45:47] framepos (frame_pos: x,y,z)
 *   [48:50] framelinvel (frame_vel)
 */

#include "mujoco_sim.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>

namespace mujoco_sim {

// ==================== SimConfig ====================

SimConfig SimConfig::LoadFromFile(const std::string& path) {
    SimConfig cfg;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[Config] 无法打开: " << path << ", 使用默认配置" << std::endl;
        return cfg;
    }

    std::string line;
    while (std::getline(file, line)) {
        // 去除注释
        auto comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }

        // 解析 key: value
        auto colon_pos = line.find(':');
        if (colon_pos == std::string::npos) continue;

        std::string key = line.substr(0, colon_pos);
        std::string value = line.substr(colon_pos + 1);

        // trim
        auto trim = [](std::string& s) {
            size_t start = s.find_first_not_of(" \t\r\n\"");
            size_t end = s.find_last_not_of(" \t\r\n\"");
            if (start == std::string::npos) { s = ""; return; }
            s = s.substr(start, end - start + 1);
        };
        trim(key);
        trim(value);

        if (key == "robot") {
            // robot: "xgb" → 模型目录
            cfg.robot_model_dir = value;
        } else if (key == "robot_scene") {
            cfg.scene_file = value;
        } else if (key == "udp_target_ip") {
            cfg.udp_target_ip = value;
        } else if (key == "udp_target_port") {
            cfg.udp_target_port = std::stoi(value);
        } else if (key == "enable_udp") {
            cfg.enable_udp = (value == "1" || value == "true");
        } else if (key == "enable_ecal") {
            cfg.enable_ecal = (value == "1" || value == "true");
        } else if (key == "enable_ros2") {
            cfg.enable_ros2 = (value == "1" || value == "true");
        } else if (key == "ros2_odom_topic") {
            cfg.ros2_odom_topic = value;
        } else if (key == "ros2_odom_rate_hz") {
            cfg.ros2_odom_rate_hz = std::stod(value);
        } else if (key == "sim_rate_hz") {
            cfg.sim_rate_hz = std::stod(value);
        } else if (key == "publish_rate_hz") {
            cfg.publish_rate_hz = std::stod(value);
        } else if (key == "udp_rate_hz") {
            cfg.udp_rate_hz = std::stod(value);
        } else if (key == "enable_gui") {
            cfg.enable_gui = (value == "1" || value == "true");
        }
    }

    std::cout << "[Config] 加载完成: robot=" << cfg.robot_model_dir
              << ", scene=" << cfg.scene_file
              << ", udp=" << cfg.udp_target_ip << ":" << cfg.udp_target_port
              << std::endl;
    return cfg;
}

// ==================== MujocoSim ====================

// 静态成员定义
mjModel* MujocoSim::s_model_ = nullptr;
mjData*  MujocoSim::s_data_  = nullptr;
mjvCamera MujocoSim::s_cam_;
mjvOption MujocoSim::s_opt_;
mjvScene  MujocoSim::s_scn_;
mjrContext MujocoSim::s_con_;
bool MujocoSim::s_button_left_ = false;
bool MujocoSim::s_button_middle_ = false;
bool MujocoSim::s_button_right_ = false;
double MujocoSim::s_lastx_ = 0;
double MujocoSim::s_lasty_ = 0;

MujocoSim::MujocoSim() = default;

MujocoSim::~MujocoSim() {
    Stop();
    // 等待线程结束
    if (physics_thread_.joinable()) physics_thread_.join();
    if (bridge_thread_.joinable())  bridge_thread_.join();
    if (ros2_spin_thread_.joinable()) ros2_spin_thread_.join();
    if (udp_sock_ >= 0) {
        close(udp_sock_);
    }
    if (data_) mj_deleteData(data_);
    if (model_) mj_deleteModel(model_);
}

bool MujocoSim::Initialize(const SimConfig& config) {
    config_ = config;

    // 1. 加载 MuJoCo 模型
    if (!LoadModel(config)) {
        return false;
    }

    // 2. 扫描关节布局 (通用化，支持 XGB/XGW/任意四足)
    ScanModelLayout();

    // 3. Resize SharedState 所有 buffer（全部动态，零硬编码）
    shared_state_.raw_qpos.resize(model_->nq);
    shared_state_.raw_qvel.resize(model_->nv);
    shared_state_.raw_tau.resize(model_->nu);
    shared_state_.q_joint.resize(layout_.total_active);
    shared_state_.qd_joint.resize(layout_.total_active);
    shared_state_.tau_joint.resize(layout_.total_active);
    prev_grav_comp_.resize(model_->nv);

    std::cout << "[Init] SharedState resized: nq=" << model_->nq
              << " nv=" << model_->nv << " nu=" << model_->nu
              << " active_joints=" << layout_.total_active << std::endl;

    // 4. 初始化 eCAL
    if (config_.enable_ecal) {
        eCAL::Initialize(0, nullptr, "mujoco_sim");

        ecal_pub_ = std::make_unique<eCAL::protobuf::CPublisher<robot_sdk::pb::RobotState>>(
            config_.ecal_state_topic);
        ecal_sub_ = std::make_unique<eCAL::protobuf::CSubscriber<robot_sdk::pb::RobotCmd>>(
            config_.ecal_cmd_topic);

        ecal_sub_->AddReceiveCallback(
            [this](const char* topic_name, const robot_sdk::pb::RobotCmd& msg,
                   long long time, long long clock, long long id) {
                OnRobotCmdReceived(topic_name, msg, time, clock, id);
            });

        std::cout << "[eCAL] 初始化完成: pub=" << config_.ecal_state_topic
                  << ", sub=" << config_.ecal_cmd_topic << std::endl;
    }

    // 3. 初始化 UDP
    if (config_.enable_udp) {
        if (!InitUdp()) {
            std::cerr << "[UDP] 初始化失败，禁用 UDP 输出" << std::endl;
            config_.enable_udp = false;
        }
    }

    // 4. 初始化 ROS 2 odom 发布
    if (config_.enable_ros2) {
        ros2_node_ = rclcpp::Node::make_shared("mujoco_sim");
        ros2_odom_pub_ = ros2_node_->create_publisher<nav_msgs::msg::Odometry>(
            config_.ros2_odom_topic, rclcpp::QoS(10));

        // 启动 spin 线程 (ROS 2 通信需要)
        ros2_spin_thread_ = std::thread([this]() {
            rclcpp::spin(ros2_node_);
        });

        std::cout << "[ROS2] 初始化完成: odom_topic=" << config_.ros2_odom_topic
                  << ", rate=" << config_.ros2_odom_rate_hz << "Hz" << std::endl;
    }

    return true;
}

bool MujocoSim::LoadModel(const SimConfig& config) {
    // 构建完整路径
    std::string model_path;
    if (!config.scene_file.empty()) {
        model_path = config.robot_model_dir + "/" + config.scene_file;
    } else {
        model_path = config.robot_model_dir + "/scene.xml";
    }

    // 加载
    char error[1000] = "";
    if (model_path.size() > 4 && model_path.substr(model_path.size() - 4) == ".mjb") {
        model_ = mj_loadModel(model_path.c_str(), nullptr);
    } else {
        model_ = mj_loadXML(model_path.c_str(), nullptr, error, sizeof(error));
    }

    if (!model_) {
        std::cerr << "[MuJoCo] 模型加载失败: " << model_path << std::endl;
        if (error[0]) std::cerr << "  错误: " << error << std::endl;
        return false;
    }

    data_ = mj_makeData(model_);
    if (!data_) {
        std::cerr << "[MuJoCo] 无法创建 mjData" << std::endl;
        return false;
    }

    // 初始朝向: yaw=90° (面向 +y, 台阶在 +y 方向)
    // quat = (cos(π/4), 0, 0, sin(π/4)) = (0.7071, 0, 0, 0.7071)
    data_->qpos[3] = 0.70710678f;  // w
    data_->qpos[4] = 0.0f;          // x
    data_->qpos[5] = 0.0f;          // y
    data_->qpos[6] = 0.70710678f;  // z

    // 初始化前向运动学
    mj_forward(model_, data_);

    std::cout << "[MuJoCo] 模型加载成功: " << model_path << std::endl;
    std::cout << "  MuJoCo 版本: " << mj_versionString() << std::endl;
    PrintModelInfo();

    return true;
}

void MujocoSim::PrintModelInfo() {
    std::cout << "\n<<------------- Model Info ------------->>" << std::endl;
    std::cout << "  nq (qpos维度): " << model_->nq << std::endl;
    std::cout << "  nv (qvel维度): " << model_->nv << std::endl;
    std::cout << "  nu (执行器数): " << model_->nu << std::endl;
    std::cout << "  njnt (关节数): " << model_->njnt << std::endl;
    std::cout << "  nbody (刚体数): " << model_->nbody << std::endl;
    std::cout << "  nsensor (传感器数): " << model_->nsensor << std::endl;
    std::cout << "  nsensordata (传感器数据维度): " << model_->nsensordata << std::endl;
    std::cout << "  timestep: " << model_->opt.timestep << std::endl;

    // 打印关节
    std::cout << "\n<<------------- Joint ------------->>" << std::endl;
    for (int i = 0; i < model_->njnt; i++) {
        const char* name = mj_id2name(model_, mjOBJ_JOINT, i);
        std::cout << "  joint[" << i << "]: "
                  << (name ? name : "(null)")
                  << " (qpos_adr=" << model_->jnt_qposadr[i] << ")" << std::endl;
    }

    // 打印执行器
    std::cout << "\n<<------------- Actuator ------------->>" << std::endl;
    for (int i = 0; i < model_->nu; i++) {
        const char* name = mj_id2name(model_, mjOBJ_ACTUATOR, i);
        std::cout << "  actuator[" << i << "]: "
                  << (name ? name : "(null)") << std::endl;
    }

    // 打印传感器
    std::cout << "\n<<------------- Sensor ------------->>" << std::endl;
    for (int i = 0; i < model_->nsensor; i++) {
        const char* name = mj_id2name(model_, mjOBJ_SENSOR, i);
        std::cout << "  sensor[" << i << "]: "
                  << (name ? name : "(null)")
                  << " (dim=" << model_->sensor_dim[i]
                  << ", adr=" << model_->sensor_adr[i] << ")" << std::endl;
    }
    std::cout << std::endl;
}

void MujocoSim::ScanModelLayout() {
    layout_ = ModelLayout{};  // reset

    // ---- 1. 扫描所有非 free 关节，按名字关键字分类 ----
    for (int i = 0; i < model_->njnt; i++) {
        if (model_->jnt_type[i] == mjJNT_FREE) continue;

        const char* cname = mj_id2name(model_, mjOBJ_JOINT, i);
        std::string name = cname ? cname : "";
        // 全部转大写匹配（兼容 FAR_ABAD / abad_joint / ...）
        std::string upper;
        upper.reserve(name.size());
        for (char c : name) upper.push_back(std::toupper(static_cast<unsigned char>(c)));

        JointEntry entry;
        entry.name = name;
        entry.jnt_id = i;
        entry.qpos_adr = model_->jnt_qposadr[i];
        entry.qvel_adr = model_->jnt_dofadr[i];

        if (upper.find("ABAD") != std::string::npos) {
            entry.role = JointRole::Abad;
            layout_.abad.push_back(entry);
        } else if (upper.find("HIP") != std::string::npos) {
            entry.role = JointRole::Hip;
            layout_.hip.push_back(entry);
        } else if (upper.find("KNEE") != std::string::npos) {
            entry.role = JointRole::Knee;
            layout_.knee.push_back(entry);
        } else if (upper.find("FOOT") != std::string::npos) {
            entry.role = JointRole::Foot;
            layout_.foot.push_back(entry);
        } else {
            entry.role = JointRole::Other;
        }
    }

    layout_.total_active = static_cast<int>(
        layout_.abad.size() + layout_.hip.size() + layout_.knee.size());

    // ---- 1.5 按腿名 canonical 顺序排序, 保证三组 (ABAD/HIP/KNEE) 腿序一致 ----
    // 老代码硬编码腿序 [FR, FL, RR, RL] (也即 XML 中的 FAR, FBL, RAR, RBL)
    // mc_ctrl policy / BuildRobotState 都假设这个顺序.
    // 从关节名提取腿名前缀: FAR/FBL/RAR/RBL → canonical index 0/1/2/3
    // 也兼容 go2 的 FL_FR_RL_RR 等前缀.
    auto legCanonicalIdx = [](const std::string& jname) -> int {
        // 先转大写提取腿前缀 (最长 3 字符)
        std::string upper;
        upper.reserve(jname.size());
        for (char c : jname) upper.push_back(std::toupper(static_cast<unsigned char>(c)));
        // 匹配常见前缀: FAR=FR 0, FBL=FL 1, RAR=RR 2, RBL=RL 3
        // 或者 GO2 风格: FR/FL/RR/RL 直接前缀
        if (upper.find("FAR") == 0) return 0;
        if (upper.find("FBL") == 0) return 1;
        if (upper.find("RAR") == 0) return 2;
        if (upper.find("RBL") == 0) return 3;
        if (upper.find("FR") == 0)  return 0;
        if (upper.find("FL") == 0)  return 1;
        if (upper.find("RR") == 0)  return 2;
        if (upper.find("RL") == 0)  return 3;
        return 99;  // 未知, 放最后
    };
    auto sortByLeg = [&](std::vector<JointEntry>& v) {
        std::sort(v.begin(), v.end(), [&](const JointEntry& a, const JointEntry& b) {
            return legCanonicalIdx(a.name) < legCanonicalIdx(b.name);
        });
    };
    sortByLeg(layout_.abad);
    sortByLeg(layout_.hip);
    sortByLeg(layout_.knee);
    sortByLeg(layout_.foot);

    // ---- 1.6 计算腿数与每腿关节数 (决定 stride) ----
    layout_.num_legs = static_cast<int>(std::min({
        layout_.abad.size(), layout_.hip.size(), layout_.knee.size()}));
    const bool has_wheel = !layout_.foot.empty()
                        && static_cast<int>(layout_.foot.size()) >= layout_.num_legs;
    layout_.jpl = has_wheel ? 4 : 3;
    layout_.total_active = layout_.num_legs * layout_.jpl;

    // ---- 2. 动态查找 joint → actuator 映射 ----
    // MuJoCo 3.x: actuator_trntype (nu x 1) 一维 int*, actuator_trnid (nu x 2) 一维 int*
    // mjTRN_JOINT=0 表示 target 是 joint, 此时 trnid[a*2] 就是 joint id
    auto bindActuator = [&](std::vector<JointEntry>& group) {
        for (auto& e : group) {
            e.actuator_id = -1;
            for (int a = 0; a < model_->nu; a++) {
                if (model_->actuator_trntype[a] == mjTRN_JOINT
                    && model_->actuator_trnid[a * 2] == e.jnt_id) {
                    e.actuator_id = a;
                    break;
                }
            }
        }
    };
    bindActuator(layout_.abad);
    bindActuator(layout_.hip);
    bindActuator(layout_.knee);
    bindActuator(layout_.foot);

    // ---- 2.5 按关节逐个绑定 torque sensor ----
    // jointactuatorfrc 的 sensor_objtype/objid 指向 **joint** (不是 actuator),
    // 因此按 jnt_id 精确匹配，得到每关节独立的 sensordata 偏移。
    // 这样不再依赖 "每腿 3 个 / 每腿 4 个" 的顺序假设。
    auto bindTauSensor = [&](std::vector<JointEntry>& group) {
        for (auto& e : group) {
            e.tau_sensor_adr = -1;
            for (int i = 0; i < model_->nsensor; i++) {
                if (model_->sensor_type[i] != mjSENS_JOINTACTFRC) continue;
                if (model_->sensor_objtype[i] != mjOBJ_JOINT) continue;
                if (model_->sensor_objid[i] != e.jnt_id) continue;
                e.tau_sensor_adr = model_->sensor_adr[i];
                break;
            }
        }
    };
    bindTauSensor(layout_.abad);
    bindTauSensor(layout_.hip);
    bindTauSensor(layout_.knee);
    bindTauSensor(layout_.foot);

    // ---- 3. 动态查找所有连续 jointactuatorfrc sensor 的合并范围 ----
    // XGB 写法: 12 个独立的 <jointactuatorfrc name=... joint=... />，每个 dim=1, adr 连续
    // XGW 写法: 可能一个 batch sensor 覆盖所有 16 关节, dim=16
    // 两种情况都要支持 —— 扫描所有 JOINTACTFRC sensor，取首尾 adr+dim 得到覆盖范围
    {
        int first_adr = -1, last_end = 0;
        for (int i = 0; i < model_->nsensor; i++) {
            if (model_->sensor_type[i] == mjSENS_JOINTACTFRC) {
                int adr = model_->sensor_adr[i];
                int end = adr + model_->sensor_dim[i];
                if (first_adr < 0) first_adr = adr;
                if (end > last_end) last_end = end;
            }
        }
        if (first_adr >= 0) {
            layout_.tau_sensor_adr = first_adr;
            layout_.tau_sensor_dim = last_end - first_adr;
        }
    }

    // ---- 4. 按名字解析 IMU / base 传感器偏移 ----
    // 不同模型关节数不同 → 传感器区整体平移, 绝不能沿用固定偏移.
    auto sensorAdr = [&](const char* name) -> int {
        int id = mj_name2id(model_, mjOBJ_SENSOR, name);
        return (id >= 0) ? model_->sensor_adr[id] : -1;
    };
    layout_.sensor_quat_adr     = sensorAdr("imu_quat");
    layout_.sensor_gyro_adr     = sensorAdr("imu_gyro");
    layout_.sensor_acc_adr      = sensorAdr("imu_acc");
    layout_.sensor_framepos_adr = sensorAdr("frame_pos");
    layout_.sensor_framevel_adr = sensorAdr("frame_vel");

    // 诊断输出
    std::cout << "[ScanModelLayout] 关节扫描结果:" << std::endl;
    auto dump = [&](const char* label, const std::vector<JointEntry>& v) {
        std::cout << "  " << label << " (" << v.size() << "):";
        for (auto& e : v) std::cout << " " << e.name
                                    << "(qpos=" << e.qpos_adr
                                    << ",qvel=" << e.qvel_adr
                                    << ",tau=" << e.tau_sensor_adr
                                    << ",act=" << e.actuator_id << ")";
        std::cout << std::endl;
    };
    dump("ABAD", layout_.abad);
    dump("HIP ", layout_.hip);
    dump("KNEE", layout_.knee);
    dump("FOOT", layout_.foot);
    std::cout << "  tau_sensor: adr=" << layout_.tau_sensor_adr
              << " dim=" << layout_.tau_sensor_dim << std::endl;
    std::cout << "  layout: num_legs=" << layout_.num_legs
              << " jpl=" << layout_.jpl
              << " total_active=" << layout_.total_active << std::endl;
    std::cout << "  sensors: quat=" << layout_.sensor_quat_adr
              << " gyro=" << layout_.sensor_gyro_adr
              << " acc=" << layout_.sensor_acc_adr
              << " framepos=" << layout_.sensor_framepos_adr
              << " framevel=" << layout_.sensor_framevel_adr << std::endl;
}

void MujocoSim::Run() {
    if (config_.enable_gui) {
        RunWithGui();
    } else {
        RunHeadless();
    }
}

void MujocoSim::RunHeadless() {
    running_.store(true);

    // SharedState 向量已在 Initialize 中 resize 到动态尺寸
    // 这里只做值初始化 (zero-fill)
    {
        std::lock_guard<std::mutex> lock(state_mutex_);
        std::fill(shared_state_.q_joint.begin(), shared_state_.q_joint.end(), 0.0f);
        std::fill(shared_state_.qd_joint.begin(), shared_state_.qd_joint.end(), 0.0f);
        std::fill(shared_state_.tau_joint.begin(), shared_state_.tau_joint.end(), 0.0f);
        shared_state_.base_position.resize(3, 0.0f);
        shared_state_.base_quat.resize(4, 0.0f);
        shared_state_.base_quat[0] = 1.0f;  // w=1 (identity)
        shared_state_.imu_gyro.resize(3, 0.0f);
        shared_state_.imu_acc.resize(3, 0.0f);
        shared_state_.base_linvel.resize(3, 0.0f);
    }

    // 启动 CarlaSdkBridgeThread (通信桥接)
    std::cout << "[CarlaSdkBridgeThread] 启动通信桥接线程..." << std::endl;
    bridge_thread_ = std::thread(&MujocoSim::CarlaSdkBridgeThreadFunc, this);

    // 启动 PhysicsThread (物理仿真)
    std::cout << "[PhysicsThread] 启动物理仿真线程 (rate=" << config_.sim_rate_hz << " Hz)..." << std::endl;
    physics_thread_ = std::thread(&MujocoSim::PhysicsThreadFunc, this);

    // 等待线程结束
    physics_thread_.join();
    bridge_thread_.join();

    std::cout << "[MujocoSim] 仿真结束, step=" << step_count_
              << ", eCAL_pub=" << publish_count_
              << ", udp_send=" << udp_send_count_ << std::endl;
}

void MujocoSim::RunWithGui() {
    running_.store(true);

    // 设置静态指针（GLFW 回调用）
    s_model_ = model_;
    s_data_  = data_;

    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "[GUI] GLFW 初始化失败, 回退到 headless 模式" << std::endl;
        RunHeadless();
        return;
    }

    // Build window title from actual loaded model.
    // MuJoCo 3.x mjModel has no .name field; use the robot dir basename from config
    // (e.g. "/.../Content/model/xgw" → "xgw"). Fallback to "unknown".
    std::string model_label = "unknown";
    if (!config_.robot_model_dir.empty()) {
        auto slash = config_.robot_model_dir.find_last_of("/");
        if (slash != std::string::npos && slash + 1 < config_.robot_model_dir.size()) {
            model_label = config_.robot_model_dir.substr(slash + 1);
        } else {
            model_label = config_.robot_model_dir;
        }
    }
    std::string window_title = "mujoco_sim - " + model_label;

    GLFWwindow* window = glfwCreateWindow(1280, 960, window_title.c_str(), NULL, NULL);
    if (!window) {
        std::cerr << "[GUI] 窗口创建失败, 回退到 headless 模式" << std::endl;
        glfwTerminate();
        RunHeadless();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // vsync

    // 初始化可视化
    mjv_defaultCamera(&s_cam_);
    mjv_defaultOption(&s_opt_);
    mjv_defaultScene(&s_scn_);
    mjr_defaultContext(&s_con_);
    mjv_makeScene(model_, &s_scn_, 2000);
    mjr_makeContext(model_, &s_con_, mjFONTSCALE_150);

    // 初始相机位置
    s_cam_.azimuth = 130;
    s_cam_.elevation = -20;
    s_cam_.distance = 1.5;
    s_cam_.lookat[0] = 0;
    s_cam_.lookat[1] = 0;
    s_cam_.lookat[2] = 0.3;

    // 注册回调
    glfwSetKeyCallback(window, GuiKeyboard);
    glfwSetCursorPosCallback(window, GuiMouseMove);
    glfwSetMouseButtonCallback(window, GuiMouseButton);
    glfwSetScrollCallback(window, GuiScroll);

    std::cout << "[GUI] 窗口已打开. ESC=退出, Backspace=重置, 鼠标旋转/缩放" << std::endl;

    // SharedState 向量已在 Initialize 中 resize 到动态尺寸
    // 这里只做值初始化 (zero-fill)
    {
        std::lock_guard<std::mutex> lock(state_mutex_);
        std::fill(shared_state_.q_joint.begin(), shared_state_.q_joint.end(), 0.0f);
        std::fill(shared_state_.qd_joint.begin(), shared_state_.qd_joint.end(), 0.0f);
        std::fill(shared_state_.tau_joint.begin(), shared_state_.tau_joint.end(), 0.0f);
        shared_state_.base_position.resize(3, 0.0f);
        shared_state_.base_quat.resize(4, 0.0f);
        shared_state_.base_quat[0] = 1.0f;
        shared_state_.imu_gyro.resize(3, 0.0f);
        shared_state_.imu_acc.resize(3, 0.0f);
        shared_state_.base_linvel.resize(3, 0.0f);
    }

    // 启动 CarlaSdkBridgeThread (通信桥接)
    std::cout << "[CarlaSdkBridgeThread] 启动通信桥接线程..." << std::endl;
    bridge_thread_ = std::thread(&MujocoSim::CarlaSdkBridgeThreadFunc, this);

    // 启动 PhysicsThread (500Hz 物理仿真)
    std::cout << "[PhysicsThread] 启动物理仿真线程 (rate=" << config_.sim_rate_hz << " Hz)..." << std::endl;
    physics_thread_ = std::thread(&MujocoSim::PhysicsThreadFunc, this);

    // 渲染主循环 (~60fps)
    while (!glfwWindowShouldClose(window) && running_.load()) {
        mjrRect viewport = {0, 0, 0, 0};
        glfwGetFramebufferSize(window, &viewport.width, &viewport.height);

        mjv_updateScene(model_, data_, &s_opt_, NULL, &s_cam_, mjCAT_ALL, &s_scn_);
        mjr_render(viewport, &s_scn_, &s_con_);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 停止线程
    running_.store(false);
    physics_thread_.join();
    bridge_thread_.join();

    // 清理
    mjv_freeScene(&s_scn_);
    mjr_freeContext(&s_con_);
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "[MujocoSim] 仿真结束, step=" << step_count_
              << ", eCAL_pub=" << publish_count_
              << ", udp_send=" << udp_send_count_ << std::endl;
}

// ==================== GLFW 回调 ====================

void MujocoSim::GuiKeyboard(GLFWwindow* window, int key, int scancode, int act, int mods) {
    if (act == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (act == GLFW_PRESS && key == GLFW_KEY_BACKSPACE) {
        if (s_model_ && s_data_) {
            mj_resetData(s_model_, s_data_);
            mj_forward(s_model_, s_data_);
        }
    }
}

void MujocoSim::GuiMouseButton(GLFWwindow* window, int button, int act, int mods) {
    s_button_left_   = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)   == GLFW_PRESS);
    s_button_middle_ = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
    s_button_right_  = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)  == GLFW_PRESS);
    glfwGetCursorPos(window, &s_lastx_, &s_lasty_);
}

void MujocoSim::GuiMouseMove(GLFWwindow* window, double xpos, double ypos) {
    if (!s_button_left_ && !s_button_middle_ && !s_button_right_) return;

    double dx = xpos - s_lastx_;
    double dy = ypos - s_lasty_;
    s_lastx_ = xpos;
    s_lasty_ = ypos;

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    bool mod_shift = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                      glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

    mjtMouse action;
    if (s_button_right_) {
        action = mod_shift ? mjMOUSE_MOVE_H : mjMOUSE_MOVE_V;
    } else if (s_button_left_) {
        action = mod_shift ? mjMOUSE_ROTATE_H : mjMOUSE_ROTATE_V;
    } else {
        action = mjMOUSE_ZOOM;
    }

    mjv_moveCamera(s_model_, action, dx / height, dy / height, &s_scn_, &s_cam_);
}

void MujocoSim::GuiScroll(GLFWwindow* window, double xoffset, double yoffset) {
    mjv_moveCamera(s_model_, mjMOUSE_ZOOM, 0, -0.05 * yoffset, &s_scn_, &s_cam_);
}

void MujocoSim::Stop() {
    running_.store(false);
}

// ==================== 线程实现 ====================

void MujocoSim::PhysicsThreadFunc() {
    std::cout << "[PhysicsThread] 线程启动 (BUILD=v5_ue_match_no_gyro_bias)" << std::endl;

    const auto period = std::chrono::microseconds(
        static_cast<int64_t>(1e6 / config_.sim_rate_hz));
    auto next_time = std::chrono::steady_clock::now();

    while (running_.load()) {
        next_time += period;

        ApplyControl();
        PhysicsStep();
        WriteSharedState();
        step_count_++;

        if (step_count_ % 5000 == 0) {
            std::cout << "[PhysicsThread] step=" << step_count_
                      << ", time=" << data_->time
                      << ", base_pos=(" << data_->qpos[0] << "," << data_->qpos[1] << "," << data_->qpos[2] << ")"
                      << ", quat_w=" << data_->qpos[3]
                      << ", has_cmd=" << has_cmd_
                      << std::endl;
        }

        std::this_thread::sleep_until(next_time);
    }

    std::cout << "[PhysicsThread] 线程结束, 总步数=" << step_count_ << std::endl;
}

void MujocoSim::CarlaSdkBridgeThreadFunc() {
    std::cout << "[CarlaSdkBridgeThread] 线程启动" << std::endl;
    std::cout << "[CarlaSdkBridgeThread] eCAL=" << (config_.enable_ecal ? "ON" : "OFF")
              << ", UDP=" << (config_.enable_udp ? "ON" : "OFF")
              << ", ROS2=" << (config_.enable_ros2 ? "ON" : "OFF")
              << ", eCAL_rate=" << config_.publish_rate_hz
              << ", UDP_rate=" << config_.udp_rate_hz
              << ", ROS2_odom_rate=" << config_.ros2_odom_rate_hz << std::endl;

    // 等待首次物理数据就绪
    {
        std::unique_lock<std::mutex> lock(state_mutex_);
        while (running_.load() && shared_state_.write_seq == 0) {
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            lock.lock();
        }
    }
    std::cout << "[CarlaSdkBridgeThread] MuJoCo 数据已就绪，开始通信循环" << std::endl;

    // eCAL 发布周期
    const auto ecal_period_us = static_cast<int64_t>(1e6 / config_.publish_rate_hz);
    // UDP 发送周期
    const auto udp_period_us = static_cast<int64_t>(1e6 / config_.udp_rate_hz);
    // ROS 2 odom发布周期
    const auto ros2_odom_period_us = static_cast<int64_t>(1e6 / config_.ros2_odom_rate_hz);

    auto next_ecal_time = std::chrono::steady_clock::now();
    auto next_udp_time  = std::chrono::steady_clock::now();
    auto next_ros2_odom_time = std::chrono::steady_clock::now();
    uint64_t last_seq = 0;

    while (running_.load()) {
        auto now = std::chrono::steady_clock::now();
        bool has_new_state = false;
        robot_sdk::pb::RobotState cached_state;

        // Use dynamic sizes from the model — supports both XGB(nq=19,nv=18,nu=12)
        // and XGW(nq=23,nv=22,nu=16) and any other robot.
        const int32_t model_nq = model_ ? model_->nq : 0;
        const int32_t model_nv = model_ ? model_->nv : 0;
        const int32_t model_nu = model_ ? model_->nu : 0;

        // Cache buffers sized to actual model
        std::vector<double> cached_qpos(model_nq, 0.0);
        std::vector<double> cached_qvel(model_nv, 0.0);
        std::vector<double> cached_tau(model_nu, 0.0);

        // Also cache raw shared_state pointers (already sized to model in PhysicsStep)
        // We copy them under lock below.
        double cached_sim_time = 0;

        // 读取 SharedState (只加锁一次，供 eCAL 和 UDP 共用)
        {
            std::lock_guard<std::mutex> lock(state_mutex_);
            if (shared_state_.write_seq > last_seq) {
                last_seq = shared_state_.write_seq;
                cached_state = BuildRobotState();
                // Copy raw data — sized vectors, use .size() * sizeof(double)
                cached_sim_time = shared_state_.sim_time;
                const auto& rq = shared_state_.raw_qpos;
                const auto& rv = shared_state_.raw_qvel;
                const auto& rt = shared_state_.raw_tau;
                std::memcpy(cached_qpos.data(), rq.data(), rq.size() * sizeof(double));
                std::memcpy(cached_qvel.data(), rv.data(), rv.size() * sizeof(double));
                std::memcpy(cached_tau.data(),  rt.data(),  rt.size()  * sizeof(double));
                has_new_state = true;
            }
        }

        // eCAL 发布 (publish_rate_hz, 通常 500Hz)
        if (config_.enable_ecal && has_new_state && now >= next_ecal_time) {
            next_ecal_time += std::chrono::microseconds(ecal_period_us);
            if (next_ecal_time < now) next_ecal_time = now;

            if (ecal_pub_) {
                ecal_pub_->Send(cached_state);
                publish_count_++;
                // 每次发送都输出 protobuf 序列化大小 (高频日志，暂时注释)
                // size_t state_bytes = cached_state.ByteSizeLong();
                // std::cout << "[eCAL TX] RobotState #" << publish_count_
                //           << ": " << state_bytes << " bytes"
                //           << " | q_abad=" << cached_state.q_abad_size()
                //           << " q_hip=" << cached_state.q_hip_size()
                //           << " q_knee=" << cached_state.q_knee_size()
                //           << " q_foot=" << cached_state.q_foot_size()
                //           << " qd_abad=" << cached_state.qd_abad_size()
                //           << " qd_hip=" << cached_state.qd_hip_size()
                //           << " qd_knee=" << cached_state.qd_knee_size()
                //           << " qd_foot=" << cached_state.qd_foot_size()
                //           << " tau_abad_fb=" << cached_state.tau_abad_fb_size()
                //           << " tau_hip_fb=" << cached_state.tau_hip_fb_size()
                //           << " tau_knee_fb=" << cached_state.tau_knee_fb_size()
                //           << " tau_foot_fb=" << cached_state.tau_foot_fb_size()
                //           << " position=" << cached_state.position_size()
                //           << " quat=" << cached_state.quat_size()
                //           << " gyro=" << cached_state.gyro_size()
                //           << " acc=" << cached_state.acc_size()
                //           << " rpy=" << cached_state.rpy_size()
                //           << " v_world=" << cached_state.v_world_size()
                //           << " ts=" << cached_state.time_stamp()
                //           << std::endl;
            }
        }

        // UDP 发送: 动态尺寸二进制 (与 Matrix robot_mujoco 格式兼容，但 nq/nv/nu 为实际值)
        if (config_.enable_udp && has_new_state && now >= next_udp_time) {
            next_udp_time += std::chrono::microseconds(udp_period_us);
            if (next_udp_time < now) next_udp_time = now;

            // Dynamic buffer size: 8+4+nq*8+4+nv*8+4+nu*8 bytes
            const int32_t nq = static_cast<int32_t>(cached_qpos.size());
            const int32_t nv = static_cast<int32_t>(cached_qvel.size());
            const int32_t nu = static_cast<int32_t>(cached_tau.size());
            const size_t buf_size = 8 + 4 + nq * 8 + 4 + nv * 8 + 4 + nu * 8;

            std::vector<char> buf(buf_size, 0);
            char* ptr = buf.data();

            // sim_time (double, 8 bytes)
            std::memcpy(ptr, &cached_sim_time, 8); ptr += 8;

            // nq (int32, 4 bytes) — now DYNAMIC, e.g. 23 for XGW
            std::memcpy(ptr, &nq, 4); ptr += 4;
            std::memcpy(ptr, cached_qpos.data(), nq * sizeof(double)); ptr += nq * sizeof(double);

            // nv (int32, 4 bytes)
            std::memcpy(ptr, &nv, 4); ptr += 4;
            std::memcpy(ptr, cached_qvel.data(), nv * sizeof(double)); ptr += nv * sizeof(double);

            // nu (int32, 4 bytes)
            std::memcpy(ptr, &nu, 4); ptr += 4;
            std::memcpy(ptr, cached_tau.data(), nu * sizeof(double)); ptr += nu * sizeof(double);

            SendUdp(std::string(buf.data(), buf_size));
            udp_send_count_++;

            if (udp_send_count_ % 100 == 1) {
                std::cout << "[CarlaSdkBridgeThread] UDP #" << udp_send_count_
                          << ": " << buf_size << " bytes (nq=" << nq << " nv=" << nv
                          << " nu=" << nu << ") → "
                          << config_.udp_target_ip << ":" << config_.udp_target_port
                          << std::endl;
            }
        }

        // ROS 2 odom发布 (ros2_odom_rate_hz, 默认 50Hz)
        if (config_.enable_ros2 && has_new_state && now >= next_ros2_odom_time) {
            next_ros2_odom_time += std::chrono::microseconds(ros2_odom_period_us);
            if (next_ros2_odom_time < now) next_ros2_odom_time = now;

            if (ros2_odom_pub_) {
                nav_msgs::msg::Odometry odom;

                // 时间戳
                auto ros_now = ros2_node_->now();
                odom.header.stamp = ros_now;
                odom.header.frame_id = "world";
                odom.child_frame_id = "base_link";

                // Pose (世界坐标系)
                odom.pose.pose.position.x = shared_state_.base_position[0];
                odom.pose.pose.position.y = shared_state_.base_position[1];
                odom.pose.pose.position.z = shared_state_.base_position[2];
                odom.pose.pose.orientation.w = shared_state_.base_quat[0];
                odom.pose.pose.orientation.x = shared_state_.base_quat[1];
                odom.pose.pose.orientation.y = shared_state_.base_quat[2];
                odom.pose.pose.orientation.z = shared_state_.base_quat[3];

                // Twist: 将世界坐标系线速度转换到体坐标系 (ROS Odometry 惯例)
                // v_body = R^T * v_world, R 从四元数构建
                double vx = shared_state_.base_linvel[0];
                double vy = shared_state_.base_linvel[1];
                double vz = shared_state_.base_linvel[2];
                double qw = shared_state_.base_quat[0];
                double qx = shared_state_.base_quat[1];
                double qy = shared_state_.base_quat[2];
                double qz = shared_state_.base_quat[3];
                // R^T (世界→体) 旋转矩阵元素
                double r00 = 1 - 2*(qy*qy + qz*qz);
                double r01 = 2*(qx*qy + qw*qz);
                double r02 = 2*(qx*qz - qw*qy);
                double r10 = 2*(qx*qy - qw*qz);
                double r11 = 1 - 2*(qx*qx + qz*qz);
                double r12 = 2*(qy*qz + qw*qx);
                double r20 = 2*(qx*qz + qw*qy);
                double r21 = 2*(qy*qz - qw*qx);
                double r22 = 1 - 2*(qx*qx + qy*qy);
                odom.twist.twist.linear.x = r00*vx + r01*vy + r02*vz;
                odom.twist.twist.linear.y = r10*vx + r11*vy + r12*vz;
                odom.twist.twist.linear.z = r20*vx + r21*vy + r22*vz;

                // 角速度: 使用 imu_gyro (体坐标系, WriteSharedState 中从 cvel+R^T 计算)
                odom.twist.twist.angular.x = shared_state_.imu_gyro[0];
                odom.twist.twist.angular.y = shared_state_.imu_gyro[1];
                odom.twist.twist.angular.z = shared_state_.imu_gyro[2];

                ros2_odom_pub_->publish(odom);
                ros2_odom_count_++;

                if (ros2_odom_count_ % 50 == 1) {
                    std::cout << "[ROS2] odom #" << ros2_odom_count_
                              << " pos=(" << odom.pose.pose.position.x
                              << "," << odom.pose.pose.position.y
                              << "," << odom.pose.pose.position.z << ")"
                              << std::endl;
                }
            }
        }

        // 避免忙等: 睡到下一个最近的事件
        auto next_event = std::min({next_ecal_time, next_udp_time, next_ros2_odom_time});
        std::this_thread::sleep_until(next_event);
    }

    std::cout << "[CarlaSdkBridgeThread] 线程结束, eCAL_pub=" << publish_count_
              << ", udp_send=" << udp_send_count_
              << ", ros2_odom=" << ros2_odom_count_ << std::endl;
}

void MujocoSim::WriteSharedState() {
    std::lock_guard<std::mutex> lock(state_mutex_);

    // ---- 主动关节 (ABAD/HIP/KNEE + 可选 FOOT/轮) 位置/速度/扭矩 ----
    // q_joint 必须按 LEG-MAJOR 顺序: [leg0_abd, leg0_hip, leg0_knee, (leg0_foot), leg1_abd, ...]
    // stride = layout_.jpl (XGB=3, XGW=4)。BuildRobotState 与 policy 都依赖这个顺序。
    // torque 用每个关节自己扫描到的 sensor 偏移, 不再用 "base + out" 的连续假设.
    {
        int out = 0;
        const int num_legs = layout_.num_legs;
        const int jpl = layout_.jpl;
        const int n_sd = model_->nsensordata;

        for (int leg = 0; leg < num_legs && out < static_cast<int>(shared_state_.q_joint.size()); leg++) {
            const std::vector<JointEntry>* groups[4] = {
                &layout_.abad, &layout_.hip, &layout_.knee, &layout_.foot
            };
            for (int r = 0; r < jpl && out < static_cast<int>(shared_state_.q_joint.size()); r++) {
                const std::vector<JointEntry>& g = *groups[r];
                if (leg >= static_cast<int>(g.size())) { out++; continue; }
                const JointEntry& e = g[leg];

                shared_state_.q_joint[out]  = static_cast<float>(data_->qpos[e.qpos_adr]);
                shared_state_.qd_joint[out] = static_cast<float>(data_->qvel[e.qvel_adr]);

                // tau: 优先用该关节自己的 sensor 偏移; 退回连续区间
                int tadr = (e.tau_sensor_adr >= 0) ? e.tau_sensor_adr
                          : ((layout_.tau_sensor_adr >= 0) ? layout_.tau_sensor_adr + out : -1);
                shared_state_.tau_joint[out] =
                    (tadr >= 0 && tadr < n_sd)
                        ? static_cast<float>(data_->sensordata[tadr]) : 0.0f;

                out++;
            }
        }
    }

    // 传感器读取: 一律用 ScanModelLayout 解析出的偏移 (模型间关节数不同会整体平移)
    auto readSensor = [&](int adr, int i) -> float {
        int a = adr + i;
        return (adr >= 0 && a < model_->nsensordata)
                    ? static_cast<float>(data_->sensordata[a]) : 0.0f;
    };

    // Base position: frame_pos sensor
    for (int i = 0; i < 3; i++) {
        shared_state_.base_position[i] = readSensor(layout_.sensor_framepos_adr, i);
    }

    // Base quaternion: imu_quat sensor (w,x,y,z)
    for (int i = 0; i < 4; i++) {
        shared_state_.base_quat[i] = readSensor(layout_.sensor_quat_adr, i);
    }

    // IMU gyro: 匹配 UE SendStateToMcCtrl() — 从 cvel 读取世界系角速度, R^T 变换到体坐标系.
    // 不使用 sensordata gyro sensor (可能有 bias), 与 UE 完全一致.
    {
        int baseBody = 1; // torso
        double wx = data_->cvel[baseBody * 6 + 0];
        double wy = data_->cvel[baseBody * 6 + 1];
        double wz = data_->cvel[baseBody * 6 + 2];
        const double* R = &data_->xmat[baseBody * 9]; // row-major 3x3 (world←body)
        // Body angular velocity = R^T * world angular velocity
        shared_state_.imu_gyro[0] = static_cast<float>(R[0]*wx + R[3]*wy + R[6]*wz);
        shared_state_.imu_gyro[1] = static_cast<float>(R[1]*wx + R[4]*wy + R[7]*wz);
        shared_state_.imu_gyro[2] = static_cast<float>(R[2]*wx + R[5]*wy + R[8]*wz);
    }

    // IMU acc: imu_acc sensor
    for (int i = 0; i < 3; i++) {
        shared_state_.imu_acc[i] = readSensor(layout_.sensor_acc_adr, i);
    }

    // Base linear velocity: frame_vel sensor
    for (int i = 0; i < 3; i++) {
        shared_state_.base_linvel[i] = readSensor(layout_.sensor_framevel_adr, i);
    }

    // ---- 原始 MuJoCo 数据 (UDP 9999 渲染同步) ----
    // Copy ALL qpos/qvel/tau — sized to actual model (XGW nq=23, XGB nq=19, etc.)
    for (int i = 0; i < model_->nq; i++) {
        shared_state_.raw_qpos[i] = data_->qpos[i];
    }
    for (int i = 0; i < model_->nv; i++) {
        shared_state_.raw_qvel[i] = data_->qvel[i];
    }
    // raw_tau 按 actuator 索引存放: 用每个关节自己的 torque sensor 偏移
    {
        std::fill(shared_state_.raw_tau.begin(), shared_state_.raw_tau.end(), 0.0);
        const std::vector<JointEntry>* groups[4] = {
            &layout_.abad, &layout_.hip, &layout_.knee, &layout_.foot
        };
        for (auto* g : groups) {
            for (const auto& e : *g) {
                if (e.actuator_id < 0 || e.actuator_id >= model_->nu) continue;
                if (e.tau_sensor_adr < 0 || e.tau_sensor_adr >= model_->nsensordata) continue;
                shared_state_.raw_tau[e.actuator_id] = data_->sensordata[e.tau_sensor_adr];
            }
        }
    }

    shared_state_.sim_time = data_->time;
    shared_state_.write_seq++;
}

void MujocoSim::PhysicsStep() {
    mj_step(model_, data_);
}

void MujocoSim::ApplyControl() {
    std::lock_guard<std::mutex> lock(cmd_mutex_);
    if (!has_cmd_) {
        // 无命令时: 零力矩, 让机器人自然下落
        if (model_ && data_) {
            for (int i = 0; i < model_->nu; i++) {
                data_->ctrl[i] = 0.0;
            }
        }
        return;
    }

    const auto& cmd = latest_cmd_;

    // 检查 PD 是否激活 (任一 kp 非零即为激活; 含轮)
    bool pd_active = false;
    for (int leg = 0; leg < 4; leg++) {
        if (cmd.kp_abad_size() > leg && cmd.kp_abad(leg) != 0.0) { pd_active = true; break; }
        if (cmd.kp_hip_size() > leg && cmd.kp_hip(leg) != 0.0) { pd_active = true; break; }
        if (cmd.kp_knee_size() > leg && cmd.kp_knee(leg) != 0.0) { pd_active = true; break; }
        if (cmd.kp_foot_size() > leg && cmd.kp_foot(leg) != 0.0) { pd_active = true; break; }
    }

    if (!pd_active) {
        // PASSIVE 模式: 零力矩, 让机器人自然下落
        for (int i = 0; i < model_->nu; i++) {
            data_->ctrl[i] = 0.0;
        }
        return;
    }

    // PD 控制: 匹配 UE 内部模式 (ApplyUdpControl)
    // tau = kp * (q_des - q) - kd * qvel
    // 无 qfrc_bias 重力补偿, 无扭矩限幅 — 与 UE 内部模式完全一致.
    // mc_ctrl (RL policy) 已包含 tau_ff 和重力补偿.
    //
    // 彻底通用化:
    //   protobuf cmd 还是按 (leg=0..3) × (ABAD/HIP/KNEE) 组织 — 这是 protobuf schema 约束.
    //   但 MuJoCo 侧的 joint 索引 / actuator 索引 不再硬编码 leg*3+jntType,
    //   而是用 ScanModelLayout 扫描到的 layout_.abad[leg], layout_.hip[leg], layout_.knee[leg].
    //   这样同时支持 XGB(每腿3) 和 XGW(每腿4, 轮).

    // 辅助 lambda: 从 protobuf cmd 读指定数组的第 i 个元素 (安全访问)
    auto pbGet = [&](int i, auto accessor, double def = 0.0) -> double {
        return accessor(i, def);
    };

    auto applyJointPD = [&](const JointEntry& e, int leg_idx,
                            auto q_des_arr, auto qd_des_arr,
                            auto kp_arr, auto kd_arr, auto tau_ff_arr) {
        if (leg_idx < 0) return;
        // 用 protobuf cmd 里的数组长度来决定当前 leg_idx 是否有效
        // (protobuf 里是 repeated 字段, 通常 4 个, 但安全起见用 accessor 返回值)
        // 这里简化: 如果数组长度 <= leg_idx, 就用 0
        // protobuf RobotCmd 已经保证数组长度=4

        double q  = data_->qpos[e.qpos_adr];
        double qd = data_->qvel[e.qvel_adr];

        double target = q_des_arr(leg_idx);
        double qd_des = qd_des_arr(leg_idx);
        double kp     = kp_arr(leg_idx);
        double kd     = kd_arr(leg_idx);
        double tau_ff = tau_ff_arr(leg_idx);

        // 匹配 UE: tau = kp*(target-q) - kd*qvel + tau_ff
        if (e.actuator_id >= 0 && e.actuator_id < model_->nu) {
            data_->ctrl[e.actuator_id] = kp * (target - q) - kd * qd + tau_ff;
        }
    };

    for (int leg = 0; leg < 4; leg++) {
        // ABAD (jointType=0)
        if (leg < static_cast<int>(layout_.abad.size())) {
            applyJointPD(layout_.abad[leg], leg,
                [&](int i) -> double { return cmd.q_des_abad_size() > i ? cmd.q_des_abad(i) : 0; },
                [&](int i) -> double { return cmd.qd_des_abad_size() > i ? cmd.qd_des_abad(i) : 0; },
                [&](int i) -> double { return cmd.kp_abad_size()     > i ? cmd.kp_abad(i)     : 0; },
                [&](int i) -> double { return cmd.kd_abad_size()     > i ? cmd.kd_abad(i)     : 0; },
                [&](int i) -> double { return cmd.tau_abad_ff_size() > i ? cmd.tau_abad_ff(i)  : 0; });
        }
        // HIP (jointType=1)
        if (leg < static_cast<int>(layout_.hip.size())) {
            applyJointPD(layout_.hip[leg], leg,
                [&](int i) -> double { return cmd.q_des_hip_size()   > i ? cmd.q_des_hip(i)   : 0; },
                [&](int i) -> double { return cmd.qd_des_hip_size()  > i ? cmd.qd_des_hip(i)  : 0; },
                [&](int i) -> double { return cmd.kp_hip_size()      > i ? cmd.kp_hip(i)      : 0; },
                [&](int i) -> double { return cmd.kd_hip_size()      > i ? cmd.kd_hip(i)      : 0; },
                [&](int i) -> double { return cmd.tau_hip_ff_size()  > i ? cmd.tau_hip_ff(i)  : 0; });
        }
        // KNEE (jointType=2)
        if (leg < static_cast<int>(layout_.knee.size())) {
            applyJointPD(layout_.knee[leg], leg,
                [&](int i) -> double { return cmd.q_des_knee_size()  > i ? cmd.q_des_knee(i)  : 0; },
                [&](int i) -> double { return cmd.qd_des_knee_size() > i ? cmd.qd_des_knee(i) : 0; },
                [&](int i) -> double { return cmd.kp_knee_size()     > i ? cmd.kp_knee(i)     : 0; },
                [&](int i) -> double { return cmd.kd_knee_size()     > i ? cmd.kd_knee(i)     : 0; },
                [&](int i) -> double { return cmd.tau_knee_ff_size() > i ? cmd.tau_knee_ff(i)  : 0; });
        }
        // FOOT / 轮 (jointType=3) — 仅当模型有轮关节时执行
        if (leg < static_cast<int>(layout_.foot.size())) {
            applyJointPD(layout_.foot[leg], leg,
                [&](int i) -> double { return cmd.q_des_foot_size()  > i ? cmd.q_des_foot(i)  : 0; },
                [&](int i) -> double { return cmd.qd_des_foot_size() > i ? cmd.qd_des_foot(i) : 0; },
                [&](int i) -> double { return cmd.kp_foot_size()     > i ? cmd.kp_foot(i)     : 0; },
                [&](int i) -> double { return cmd.kd_foot_size()     > i ? cmd.kd_foot(i)     : 0; },
                [&](int i) -> double { return cmd.tau_foot_ff_size() > i ? cmd.tau_foot_ff(i)  : 0; });
        }
    }

    // 诊断日志: 每100步输出完整控制信息
    // 路径可通过环境变量 MUJOCO_DIAG_LOG 配置，默认 /tmp/mujoco_diag.log
    static FILE* diag_fp = nullptr;
    if (!diag_fp) {
        const char* diag_path = getenv("MUJOCO_DIAG_LOG");
        if (!diag_path || !diag_path[0]) diag_path = "/tmp/mujoco_diag.log";
        diag_fp = fopen(diag_path, "w");
        if (diag_fp) {
            fprintf(diag_fp, "# step time base_x base_y base_z qw qx qy qz ");
            fprintf(diag_fp, "FR_ab_q FR_ab_qdes FR_ab_qd FR_ab_ctrl FR_ab_gcomp ");
            fprintf(diag_fp, "FR_hi_q FR_hi_qdes FR_hi_qd FR_hi_ctrl FR_hi_gcomp ");
            fprintf(diag_fp, "FR_kn_q FR_kn_qdes FR_kn_qd FR_kn_ctrl FR_kn_gcomp ");
            fprintf(diag_fp, "kp_abad0 kp_hip0 kp_knee0 kd_abad0 kd_hip0 kd_knee0 ");
            fprintf(diag_fp, "tau_ff_abad0 tau_ff_hip0 tau_ff_knee0 ");
            fprintf(diag_fp, "v_world_x v_world_y v_world_z ");
            fprintf(diag_fp, "gyro_x gyro_y gyro_z acc_x acc_y acc_z\n");
            fprintf(diag_fp, "# BUILD_MARKER=v5_ue_match_no_gyro_bias\n");
        }
    }
    if (diag_fp && step_count_ % 100 == 0) {
        // base state
        fprintf(diag_fp, "%lu %.4f %.6f %.6f %.6f %.6f %.6f %.6f %.6f ",
                (unsigned long)step_count_, data_->time,
                data_->qpos[0], data_->qpos[1], data_->qpos[2],
                data_->qpos[3], data_->qpos[4], data_->qpos[5], data_->qpos[6]);
        // FR joint details (leg=0) — 用 layout_ 动态索引
        const JointEntry* diagJoints[3] = {
            (layout_.abad.size() > 0) ? &layout_.abad[0] : nullptr,
            (layout_.hip.size()  > 0) ? &layout_.hip[0]  : nullptr,
            (layout_.knee.size() > 0) ? &layout_.knee[0] : nullptr
        };
        for (int j = 0; j < 3; j++) {
            const JointEntry* e = diagJoints[j];
            double q = e ? data_->qpos[e->qpos_adr] : 0;
            double qv = e ? data_->qvel[e->qvel_adr] : 0;
            double ctrl = (e && e->actuator_id >= 0 && e->actuator_id < model_->nu)
                        ? data_->ctrl[e->actuator_id] : 0;
            double gcomp = e ? data_->qfrc_bias[e->qvel_adr] : 0;
            double qdes = 0, qdes_v = 0, tau_ff = 0, kp = 0, kd = 0;
            if (j == 0) {
                qdes = cmd.q_des_abad_size() > 0 ? cmd.q_des_abad(0) : 0;
                qdes_v = cmd.qd_des_abad_size() > 0 ? cmd.qd_des_abad(0) : 0;
                tau_ff = cmd.tau_abad_ff_size() > 0 ? cmd.tau_abad_ff(0) : 0;
                kp = cmd.kp_abad_size() > 0 ? cmd.kp_abad(0) : 0;
                kd = cmd.kd_abad_size() > 0 ? cmd.kd_abad(0) : 0;
            } else if (j == 1) {
                qdes = cmd.q_des_hip_size() > 0 ? cmd.q_des_hip(0) : 0;
                qdes_v = cmd.qd_des_hip_size() > 0 ? cmd.qd_des_hip(0) : 0;
                tau_ff = cmd.tau_hip_ff_size() > 0 ? cmd.tau_hip_ff(0) : 0;
                kp = cmd.kp_hip_size() > 0 ? cmd.kp_hip(0) : 0;
                kd = cmd.kd_hip_size() > 0 ? cmd.kd_hip(0) : 0;
            } else {
                qdes = cmd.q_des_knee_size() > 0 ? cmd.q_des_knee(0) : 0;
                qdes_v = cmd.qd_des_knee_size() > 0 ? cmd.qd_des_knee(0) : 0;
                tau_ff = 0;
                kp = cmd.kp_knee_size() > 0 ? cmd.kp_knee(0) : 0;
                kd = cmd.kd_knee_size() > 0 ? cmd.kd_knee(0) : 0;
            }
            fprintf(diag_fp, "%.6f %.6f %.6f %.6f %.6f ",
                    q, qdes, qv, ctrl, gcomp);
        }
        // gains and tau_ff for leg 0
        fprintf(diag_fp, "%.2f %.2f %.2f %.2f %.2f %.2f %.4f %.4f %.4f ",
                cmd.kp_abad_size() > 0 ? cmd.kp_abad(0) : 0.0,
                cmd.kp_hip_size() > 0 ? cmd.kp_hip(0) : 0.0,
                cmd.kp_knee_size() > 0 ? cmd.kp_knee(0) : 0.0,
                cmd.kd_abad_size() > 0 ? cmd.kd_abad(0) : 0.0,
                cmd.kd_hip_size() > 0 ? cmd.kd_hip(0) : 0.0,
                cmd.kd_knee_size() > 0 ? cmd.kd_knee(0) : 0.0,
                cmd.tau_abad_ff_size() > 0 ? cmd.tau_abad_ff(0) : 0.0,
                cmd.tau_hip_ff_size() > 0 ? cmd.tau_hip_ff(0) : 0.0,
                0.0);
        // v_world (before conversion)
        fprintf(diag_fp, "%.6f %.6f %.6f ",
                shared_state_.base_linvel[0], shared_state_.base_linvel[1], shared_state_.base_linvel[2]);
        // gyro and acc
        fprintf(diag_fp, "%.6f %.6f %.6f %.6f %.6f %.6f\n",
                shared_state_.imu_gyro[0], shared_state_.imu_gyro[1], shared_state_.imu_gyro[2],
                shared_state_.imu_acc[0], shared_state_.imu_acc[1], shared_state_.imu_acc[2]);
        fflush(diag_fp);
    }

    // 站立轨迹高频抓取: CAPTURE=1 环境变量开启
    // 每 2 步 (100Hz) 记录 base 姿态 + 12 关节 q/qdes, 用于反推 mc_ctrl 站立路点
    static FILE* cap_fp = nullptr;
    static bool cap_init = false;
    if (!cap_init) {
        cap_init = true;
        if (getenv("CAPTURE") && std::string(getenv("CAPTURE")) == "1") {
            cap_fp = fopen("/tmp/standup_capture.csv", "w");
            if (cap_fp) {
                fprintf(cap_fp, "# step time x y z qw qx qy qz");
                const char* legN[4] = {"FR", "FL", "RL", "RR"};
                for (int lg = 0; lg < 4; lg++)
                    fprintf(cap_fp, " %s_ab_q %s_ab_d %s_hi_q %s_hi_d %s_kn_q %s_kn_d",
                            legN[lg], legN[lg], legN[lg], legN[lg], legN[lg], legN[lg]);
                fprintf(cap_fp, "\n");
                printf("[CAPTURE] enabled -> /tmp/standup_capture.csv\n");
            }
        }
    }
    if (cap_fp && step_count_ % 2 == 0) {
        fprintf(cap_fp, "%lu %.4f %.6f %.6f %.6f %.6f %.6f %.6f %.6f",
                (unsigned long)step_count_, data_->time,
                data_->qpos[0], data_->qpos[1], data_->qpos[2],
                data_->qpos[3], data_->qpos[4], data_->qpos[5], data_->qpos[6]);
        for (int leg = 0; leg < 4; leg++) {  // MuJoCo 腿序: FR FL RL RR
            double ab_d = cmd.q_des_abad_size() > leg ? cmd.q_des_abad(leg) : 0;
            double hi_d = cmd.q_des_hip_size()  > leg ? cmd.q_des_hip(leg)  : 0;
            double kn_d = cmd.q_des_knee_size() > leg ? cmd.q_des_knee(leg) : 0;
            double ab_q = (leg < static_cast<int>(layout_.abad.size()))
                        ? data_->qpos[layout_.abad[leg].qpos_adr] : 0;
            double hi_q = (leg < static_cast<int>(layout_.hip.size()))
                        ? data_->qpos[layout_.hip[leg].qpos_adr] : 0;
            double kn_q = (leg < static_cast<int>(layout_.knee.size()))
                        ? data_->qpos[layout_.knee[leg].qpos_adr] : 0;
            fprintf(cap_fp, " %.6f %.6f %.6f %.6f %.6f %.6f",
                    ab_q, ab_d, hi_q, hi_d, kn_q, kn_d);
        }
        fprintf(cap_fp, "\n");
        fflush(cap_fp);
    }
}

robot_sdk::pb::RobotState MujocoSim::BuildRobotState() {
    // 注意: 调用者必须已持有 state_mutex_
    robot_sdk::pb::RobotState state;

    // 关节状态: 从 shared_state_ 读取 (stride = layout_.jpl, XGB=3, XGW=4)
    const int jpl = layout_.jpl;
    const bool has_wheel = (jpl >= 4);
    for (int leg = 0; leg < layout_.num_legs; leg++) {
        const int base = leg * jpl;
        state.add_q_abad(shared_state_.q_joint[base + 0]);
        state.add_q_hip(shared_state_.q_joint[base + 1]);
        state.add_q_knee(shared_state_.q_joint[base + 2]);
        state.add_q_foot(has_wheel ? shared_state_.q_joint[base + 3] : 0.0f);

        state.add_qd_abad(shared_state_.qd_joint[base + 0]);
        state.add_qd_hip(shared_state_.qd_joint[base + 1]);
        state.add_qd_knee(shared_state_.qd_joint[base + 2]);
        state.add_qd_foot(has_wheel ? shared_state_.qd_joint[base + 3] : 0.0f);
    }

    // 关节力矩
    for (int leg = 0; leg < layout_.num_legs; leg++) {
        const int base = leg * jpl;
        state.add_tau_abad_fb(shared_state_.tau_joint[base + 0]);
        state.add_tau_hip_fb(shared_state_.tau_joint[base + 1]);
        state.add_tau_knee_fb(shared_state_.tau_joint[base + 2]);
        state.add_tau_foot_fb(has_wheel ? shared_state_.tau_joint[base + 3] : 0.0f);
    }

    // Base position
    for (int i = 0; i < 3; i++) {
        state.add_position(shared_state_.base_position[i]);
    }

    // 四元数 (w,x,y,z)
    for (int i = 0; i < 4; i++) {
        state.add_quat(shared_state_.base_quat[i]);
    }

    // 陀螺仪: cvel + R^T 体坐标系 (WriteSharedState 中计算).
    // 匹配 UE 内部模式 SendStateToMcCtrl() 的实现.
    // 无 bias 估计, 无 EMA 跟踪, 无滤波 — 与 UE 完全一致.
    for (int i = 0; i < 3; i++) {
        state.add_gyro(shared_state_.imu_gyro[i]);
    }

    // 加速度计: 保持世界坐标系 (与原始 robot_mujoco 一致)
    // 反汇编确认原始 robot_mujoco 未对 acc 做 R^T 变换, 直接发送世界坐标系数据.
    for (int i = 0; i < 3; i++) {
        state.add_acc(shared_state_.imu_acc[i]);
    }

    // 时间戳: 不设置 (匹配原始 robot_mujoco 的 292 字节格式)
    // eCAL 抓包确认 robot_mujoco 发送 ts=0, 因为 eCAL 共享内存即时送达, 无需新鲜度检查.
    // 注意: Matrix UE 通过 UDP 发送时才使用 CLOCK_REALTIME (303 字节格式).

    // rpy: 不发送 (匹配原始 robot_mujoco 的 292 字节格式)
    // eCAL 抓包确认 robot_mujoco 不包含 rpy 字段.

    // v_world: 直接发送世界坐标系线速度
    // 与原始 Matrix/CarlaUnreal 保持一致: UE 端从 cvel 读取世界系速度直接发送,
    // mc_ctrl 和 RL 策略期望接收世界坐标系速度.
    // 之前的 v_world→v_body 转换是错误的, 会导致策略收到虚假侧向速度.
    state.add_v_world(shared_state_.base_linvel[0]);
    state.add_v_world(shared_state_.base_linvel[1]);
    state.add_v_world(shared_state_.base_linvel[2]);

    return state;
}

void MujocoSim::OnRobotCmdReceived(const char* topic_name,
                                    const robot_sdk::pb::RobotCmd& msg,
                                    long long time, long long clock, long long id) {
    std::lock_guard<std::mutex> lock(cmd_mutex_);
    latest_cmd_ = msg;
    has_cmd_ = true;

    // 每次收到命令都输出 protobuf 反序列化大小和字段信息 (高频日志，暂时注释)
    static uint64_t cmd_count = 0;
    cmd_count++;
    // {
    //     size_t cmd_bytes = msg.ByteSizeLong();
    //     std::cout << "[eCAL RX] RobotCmd #" << cmd_count
    //               << ": " << cmd_bytes << " bytes"
    //               << " | q_des_abad=" << msg.q_des_abad_size()
    //               << " q_des_hip=" << msg.q_des_hip_size()
    //               << " q_des_knee=" << msg.q_des_knee_size()
    //               << " q_des_foot=" << msg.q_des_foot_size()
    //               << " qd_des_abad=" << msg.qd_des_abad_size()
    //               << " qd_des_hip=" << msg.qd_des_hip_size()
    //               << " qd_des_knee=" << msg.qd_des_knee_size()
    //               << " qd_des_foot=" << msg.qd_des_foot_size()
    //               << " kp_abad=" << msg.kp_abad_size()
    //               << " kp_hip=" << msg.kp_hip_size()
    //               << " kp_knee=" << msg.kp_knee_size()
    //               << " kp_foot=" << msg.kp_foot_size()
    //               << " kd_abad=" << msg.kd_abad_size()
    //               << " kd_hip=" << msg.kd_hip_size()
    //               << " kd_knee=" << msg.kd_knee_size()
    //               << " kd_foot=" << msg.kd_foot_size()
    //               << " tau_abad_ff=" << msg.tau_abad_ff_size()
    //               << " tau_hip_ff=" << msg.tau_hip_ff_size()
    //               << " tau_knee_ff=" << msg.tau_knee_ff_size()
    //               << " tau_foot_ff=" << msg.tau_foot_ff_size()
    //               << std::endl;
    // }
    if (cmd_count % 500 == 1) {
        std::cout << "[eCAL] RobotCmd #" << cmd_count
                  << ": q_des_abad[0]=" << (msg.q_des_abad_size() > 0 ? msg.q_des_abad(0) : -999)
                  << " q_des_hip[0]=" << (msg.q_des_hip_size() > 0 ? msg.q_des_hip(0) : -999)
                  << " q_des_knee[0]=" << (msg.q_des_knee_size() > 0 ? msg.q_des_knee(0) : -999)
                  << " kp_abad[0]=" << (msg.kp_abad_size() > 0 ? msg.kp_abad(0) : -999)
                  << std::endl;
    }
}

bool MujocoSim::InitUdp() {
    udp_sock_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock_ < 0) {
        std::cerr << "[UDP] socket() 失败" << std::endl;
        return false;
    }

    // 目标: CarlaUnreal (port 9999, Matrix-compatible)
    std::memset(&udp_dest_, 0, sizeof(udp_dest_));
    udp_dest_.sin_family = AF_INET;
    udp_dest_.sin_port = htons(config_.udp_target_port);
    inet_pton(AF_INET, config_.udp_target_ip.c_str(), &udp_dest_.sin_addr);

    std::cout << "[UDP] 初始化完成: " << config_.udp_target_ip
              << ":" << config_.udp_target_port << " (CarlaUnreal)" << std::endl;
    return true;
}

void MujocoSim::SendUdp(const std::string& data) {
    if (udp_sock_ < 0) return;
    sendto(udp_sock_, data.c_str(), data.size(), 0,
           reinterpret_cast<struct sockaddr*>(&udp_dest_), sizeof(udp_dest_));
}

}  // namespace mujoco_sim
