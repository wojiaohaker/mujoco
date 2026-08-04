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

    // 2. 初始化 eCAL
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

void MujocoSim::Run() {
    if (config_.enable_gui) {
        RunWithGui();
    } else {
        RunHeadless();
    }
}

void MujocoSim::RunHeadless() {
    running_.store(true);

    // 初始化 SharedState 向量
    {
        std::lock_guard<std::mutex> lock(state_mutex_);
        shared_state_.q_joint.resize(12, 0.0f);
        shared_state_.qd_joint.resize(12, 0.0f);
        shared_state_.tau_joint.resize(12, 0.0f);
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

    GLFWwindow* window = glfwCreateWindow(1280, 960, "mujoco_sim - XGB", NULL, NULL);
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

    // 初始化 SharedState 向量
    {
        std::lock_guard<std::mutex> lock(state_mutex_);
        shared_state_.q_joint.resize(12, 0.0f);
        shared_state_.qd_joint.resize(12, 0.0f);
        shared_state_.tau_joint.resize(12, 0.0f);
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
    std::cout << "[PhysicsThread] 线程启动" << std::endl;

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
              << ", eCAL_rate=" << config_.publish_rate_hz
              << ", UDP_rate=" << config_.udp_rate_hz << std::endl;

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

    auto next_ecal_time = std::chrono::steady_clock::now();
    auto next_udp_time  = std::chrono::steady_clock::now();
    uint64_t last_seq = 0;

    while (running_.load()) {
        auto now = std::chrono::steady_clock::now();
        bool has_new_state = false;
        robot_sdk::pb::RobotState cached_state;
        // 缓存原始数据 (锁内拷贝)
        double cached_sim_time = 0;
        double cached_qpos[19] = {};
        double cached_qvel[18] = {};
        double cached_tau[12] = {};

        // 读取 SharedState (只加锁一次，供 eCAL 和 UDP 共用)
        {
            std::lock_guard<std::mutex> lock(state_mutex_);
            if (shared_state_.write_seq > last_seq) {
                last_seq = shared_state_.write_seq;
                cached_state = BuildRobotState();
                // 拷贝原始数据
                cached_sim_time = shared_state_.sim_time;
                std::memcpy(cached_qpos, shared_state_.raw_qpos, sizeof(cached_qpos));
                std::memcpy(cached_qvel, shared_state_.raw_qvel, sizeof(cached_qvel));
                std::memcpy(cached_tau,  shared_state_.raw_tau,  sizeof(cached_tau));
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
            }
        }

        // UDP 发送: 412 字节原始二进制 (与 Matrix robot_mujoco 一致)
        if (config_.enable_udp && has_new_state && now >= next_udp_time) {
            next_udp_time += std::chrono::microseconds(udp_period_us);
            if (next_udp_time < now) next_udp_time = now;

            // 打包 412 bytes: sim_time(8) + nq(4) + qpos(19*8) + nv(4) + qvel(18*8) + nu(4) + tau(12*8)
            char buf[412];
            std::memset(buf, 0, sizeof(buf));
            char* ptr = buf;

            // sim_time (double, 8 bytes)
            std::memcpy(ptr, &cached_sim_time, 8); ptr += 8;

            // nq (int32, 4 bytes) = 19
            int32_t nq = 19;
            std::memcpy(ptr, &nq, 4); ptr += 4;

            // qpos (19 doubles, 152 bytes)
            std::memcpy(ptr, cached_qpos, 19 * sizeof(double)); ptr += 19 * sizeof(double);

            // nv (int32, 4 bytes) = 18
            int32_t nv = 18;
            std::memcpy(ptr, &nv, 4); ptr += 4;

            // qvel (18 doubles, 144 bytes)
            std::memcpy(ptr, cached_qvel, 18 * sizeof(double)); ptr += 18 * sizeof(double);

            // nu (int32, 4 bytes) = 12
            int32_t nu = 12;
            std::memcpy(ptr, &nu, 4); ptr += 4;

            // tau (12 doubles, 96 bytes)
            std::memcpy(ptr, cached_tau, 12 * sizeof(double)); ptr += 12 * sizeof(double);

            SendUdp(std::string(buf, 412));
            udp_send_count_++;

            if (udp_send_count_ % 100 == 1) {
                std::cout << "[CarlaSdkBridgeThread] UDP #" << udp_send_count_
                          << ": 412 bytes (raw MuJoCo) → "
                          << config_.udp_target_ip << ":" << config_.udp_target_port
                          << std::endl;
            }
        }

        // 避免忙等: 睡到下一个最近的事件
        auto next_event = std::min(next_ecal_time, next_udp_time);
        std::this_thread::sleep_until(next_event);
    }

    std::cout << "[CarlaSdkBridgeThread] 线程结束, eCAL_pub=" << publish_count_
              << ", udp_send=" << udp_send_count_ << std::endl;
}

void MujocoSim::WriteSharedState() {
    std::lock_guard<std::mutex> lock(state_mutex_);

    // 关节位置 qpos[7:18] → q_joint[0:11]
    for (int leg = 0; leg < 4; leg++) {
        int qpos_idx = 7 + leg * 3;
        int vel_idx  = 6 + leg * 3;
        int tau_idx  = 24 + leg * 3;

        shared_state_.q_joint[leg * 3 + 0] = static_cast<float>(data_->qpos[qpos_idx + 0]);
        shared_state_.q_joint[leg * 3 + 1] = static_cast<float>(data_->qpos[qpos_idx + 1]);
        shared_state_.q_joint[leg * 3 + 2] = static_cast<float>(data_->qpos[qpos_idx + 2]);

        shared_state_.qd_joint[leg * 3 + 0] = static_cast<float>(data_->qvel[vel_idx + 0]);
        shared_state_.qd_joint[leg * 3 + 1] = static_cast<float>(data_->qvel[vel_idx + 1]);
        shared_state_.qd_joint[leg * 3 + 2] = static_cast<float>(data_->qvel[vel_idx + 2]);

        shared_state_.tau_joint[leg * 3 + 0] = static_cast<float>(data_->sensordata[tau_idx + 0]);
        shared_state_.tau_joint[leg * 3 + 1] = static_cast<float>(data_->sensordata[tau_idx + 1]);
        shared_state_.tau_joint[leg * 3 + 2] = static_cast<float>(data_->sensordata[tau_idx + 2]);
    }

    // Base position: sensordata[46:48] (frame_pos, adr=46)
    shared_state_.base_position[0] = static_cast<float>(data_->sensordata[46]);
    shared_state_.base_position[1] = static_cast<float>(data_->sensordata[47]);
    shared_state_.base_position[2] = static_cast<float>(data_->sensordata[48]);

    // Base quaternion: sensordata[36:39] (imu_quat, adr=36, w,x,y,z)
    shared_state_.base_quat[0] = static_cast<float>(data_->sensordata[36]);
    shared_state_.base_quat[1] = static_cast<float>(data_->sensordata[37]);
    shared_state_.base_quat[2] = static_cast<float>(data_->sensordata[38]);
    shared_state_.base_quat[3] = static_cast<float>(data_->sensordata[39]);

    // IMU gyro: sensordata[40:42] (imu_gyro, adr=40)
    shared_state_.imu_gyro[0] = static_cast<float>(data_->sensordata[40]);
    shared_state_.imu_gyro[1] = static_cast<float>(data_->sensordata[41]);
    shared_state_.imu_gyro[2] = static_cast<float>(data_->sensordata[42]);

    // IMU acc: sensordata[43:45] (imu_acc, adr=43)
    shared_state_.imu_acc[0] = static_cast<float>(data_->sensordata[43]);
    shared_state_.imu_acc[1] = static_cast<float>(data_->sensordata[44]);
    shared_state_.imu_acc[2] = static_cast<float>(data_->sensordata[45]);

    // Base linear velocity: sensordata[49:51] (frame_vel, adr=49)
    shared_state_.base_linvel[0] = static_cast<float>(data_->sensordata[49]);
    shared_state_.base_linvel[1] = static_cast<float>(data_->sensordata[50]);
    shared_state_.base_linvel[2] = static_cast<float>(data_->sensordata[51]);

    // ---- 原始 MuJoCo 数据 (UDP 9999 渲染同步) ----
    // raw_qpos[0:2] = base_pos, [3:6] = base_quat(w,x,y,z), [7:18] = joints
    for (int i = 0; i < model_->nq && i < 19; i++) {
        shared_state_.raw_qpos[i] = data_->qpos[i];
    }
    for (int i = 0; i < model_->nv && i < 18; i++) {
        shared_state_.raw_qvel[i] = data_->qvel[i];
    }
    // tau: sensordata[24:35] = jointactuatorfrc (12 joints)
    for (int i = 0; i < 12; i++) {
        shared_state_.raw_tau[i] = data_->sensordata[24 + i];
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
        // 无命令时: 零力矩, 让机器人自然下落 (与 robot_mujoco 一致)
        if (model_ && data_) {
            for (int i = 0; i < model_->nu; i++) {
                data_->ctrl[i] = 0.0;
            }
        }
        return;
    }

    const auto& cmd = latest_cmd_;

    // 检查 PD 是否激活 (任一 kp 非零即为激活)
    // PASSIVE 模式发送全零命令 (kp=0), 此时不应加 qfrc_bias, 否则机器人会被向上推
    bool pd_active = false;
    for (int leg = 0; leg < 4; leg++) {
        if (cmd.kp_abad_size() > leg && cmd.kp_abad(leg) != 0.0) { pd_active = true; break; }
        if (cmd.kp_hip_size() > leg && cmd.kp_hip(leg) != 0.0) { pd_active = true; break; }
        if (cmd.kp_knee_size() > leg && cmd.kp_knee(leg) != 0.0) { pd_active = true; break; }
    }

    if (!pd_active) {
        // PASSIVE 模式: 零力矩, 让机器人自然下落
        for (int i = 0; i < model_->nu; i++) {
            data_->ctrl[i] = 0.0;
        }
        return;
    }

    // PD 激活 (STANDUP / RL 模式): 纯 PD 控制律
    // 注意: 不调用 mj_forward, mj_step 内部会自动计算 qfrc_bias
    // tau = kp*(q_des - q) + kd*(qd_des - qd) + tau_ff
    // 关节顺序: [FR_abad, FR_hip, FR_knee, FL_abad, FL_hip, FL_knee,
    //            RR_abad, RR_hip, RR_knee, RL_abad, RL_hip, RL_knee]
    //
    // qpos: [0:6]=freejoint(pos+quat), [7:18]=12个关节
    // qvel: [0:5]=freejoint(linvel+angvel), [6:17]=12个关节

    for (int leg = 0; leg < 4; leg++) {
        int jnt_idx = 7 + leg * 3;   // qpos 中的关节起始索引
        int vel_idx = 6 + leg * 3;   // qvel 中的关节起始索引
        int ctrl_idx = leg * 3;      // ctrl 中的执行器起始索引

        // abad
        if (cmd.q_des_abad_size() > leg && cmd.kp_abad_size() > leg) {
            double q = data_->qpos[jnt_idx + 0];
            double qd = data_->qvel[vel_idx + 0];
            double tau = cmd.kp_abad(leg) * (cmd.q_des_abad(leg) - q)
                       + cmd.kd_abad(leg) * (cmd.qd_des_abad_size() > leg ? cmd.qd_des_abad(leg) : 0.0 - qd)
                       + (cmd.tau_abad_ff_size() > leg ? cmd.tau_abad_ff(leg) : 0.0);
            data_->ctrl[ctrl_idx + 0] = tau;
        }

        // hip
        if (cmd.q_des_hip_size() > leg && cmd.kp_hip_size() > leg) {
            double q = data_->qpos[jnt_idx + 1];
            double qd = data_->qvel[vel_idx + 1];
            double tau = cmd.kp_hip(leg) * (cmd.q_des_hip(leg) - q)
                       + cmd.kd_hip(leg) * (cmd.qd_des_hip_size() > leg ? cmd.qd_des_hip(leg) : 0.0 - qd)
                       + (cmd.tau_hip_ff_size() > leg ? cmd.tau_hip_ff(leg) : 0.0);
            data_->ctrl[ctrl_idx + 1] = tau;
        }

        // knee
        if (cmd.q_des_knee_size() > leg && cmd.kp_knee_size() > leg) {
            double q = data_->qpos[jnt_idx + 2];
            double qd = data_->qvel[vel_idx + 2];
            double tau = cmd.kp_knee(leg) * (cmd.q_des_knee(leg) - q)
                       + cmd.kd_knee(leg) * (cmd.qd_des_knee_size() > leg ? cmd.qd_des_knee(leg) : 0.0 - qd)
                       + 0.0;  // knee 没有 tau_ff 字段
            data_->ctrl[ctrl_idx + 2] = tau;
        }
    }
}

robot_sdk::pb::RobotState MujocoSim::BuildRobotState() {
    // 注意: 调用者必须已持有 state_mutex_
    robot_sdk::pb::RobotState state;

    // 关节状态: 从 shared_state_ 读取
    for (int leg = 0; leg < 4; leg++) {
        state.add_q_abad(shared_state_.q_joint[leg * 3 + 0]);
        state.add_q_hip(shared_state_.q_joint[leg * 3 + 1]);
        state.add_q_knee(shared_state_.q_joint[leg * 3 + 2]);
        state.add_q_foot(0.0f);

        state.add_qd_abad(shared_state_.qd_joint[leg * 3 + 0]);
        state.add_qd_hip(shared_state_.qd_joint[leg * 3 + 1]);
        state.add_qd_knee(shared_state_.qd_joint[leg * 3 + 2]);
        state.add_qd_foot(0.0f);
    }

    // 关节力矩
    for (int leg = 0; leg < 4; leg++) {
        state.add_tau_abad_fb(shared_state_.tau_joint[leg * 3 + 0]);
        state.add_tau_hip_fb(shared_state_.tau_joint[leg * 3 + 1]);
        state.add_tau_knee_fb(shared_state_.tau_joint[leg * 3 + 2]);
    }

    // Base position
    for (int i = 0; i < 3; i++) {
        state.add_position(shared_state_.base_position[i]);
    }

    // tau_foot_fb (填0, xgb 无足端力矩传感器)
    for (int i = 0; i < 4; i++) {
        state.add_tau_foot_fb(0.0f);
    }

    // 四元数 (w,x,y,z)
    for (int i = 0; i < 4; i++) {
        state.add_quat(shared_state_.base_quat[i]);
    }

    // 陀螺仪
    for (int i = 0; i < 3; i++) {
        state.add_gyro(shared_state_.imu_gyro[i]);
    }

    // 加速度计
    for (int i = 0; i < 3; i++) {
        state.add_acc(shared_state_.imu_acc[i]);
    }

    // 时间戳 (纳秒)
    auto now = std::chrono::steady_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        now.time_since_epoch()).count();
    state.set_time_stamp(static_cast<uint64_t>(ns));

    // rpy (从四元数计算)
    double w = shared_state_.base_quat[0];
    double x = shared_state_.base_quat[1];
    double y = shared_state_.base_quat[2];
    double z = shared_state_.base_quat[3];
    double roll  = std::atan2(2.0 * (w * x + y * z), 1.0 - 2.0 * (x * x + y * y));
    double sinp  = 2.0 * (w * y - z * x);
    double pitch = (std::abs(sinp) >= 1.0) ?
        std::copysign(M_PI / 2, sinp) : std::asin(sinp);
    double yaw   = std::atan2(2.0 * (w * z + x * y), 1.0 - 2.0 * (y * y + z * z));

    state.add_rpy(static_cast<float>(roll));
    state.add_rpy(static_cast<float>(pitch));
    state.add_rpy(static_cast<float>(yaw));

    // v_world (base linear velocity)
    for (int i = 0; i < 3; i++) {
        state.add_v_world(shared_state_.base_linvel[i]);
    }

    return state;
}

void MujocoSim::OnRobotCmdReceived(const char* topic_name,
                                    const robot_sdk::pb::RobotCmd& msg,
                                    long long time, long long clock, long long id) {
    std::lock_guard<std::mutex> lock(cmd_mutex_);
    latest_cmd_ = msg;
    has_cmd_ = true;

    // 调试: 每次收到命令计数
    static uint64_t cmd_count = 0;
    cmd_count++;
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
