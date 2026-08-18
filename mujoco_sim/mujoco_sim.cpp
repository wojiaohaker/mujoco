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

    // 关节位置 qpos[7:18] → q_joint[0:11]
    for (int leg = 0; leg < 4; leg++) {
        int qpos_idx = 7 + leg * 3;
        int vel_idx  = 6 + leg * 3;
        int tau_idx  = 24 + leg * 3;

        shared_state_.q_joint[leg * 3 + 0] = static_cast<float>(data_->qpos[qpos_idx + 0]);
        shared_state_.q_joint[leg * 3 + 1] = static_cast<float>(data_->qpos[qpos_idx + 1]);
        shared_state_.q_joint[leg * 3 + 2] = static_cast<float>(data_->qpos[qpos_idx + 2]);

        // 直接使用原始 qvel (不使用低通滤波)
        // 原因: alpha=0.2 的指数滤波器在 RL policy 频率(200Hz)处引入~86°相位滞后,
        // 导致 LSTM 隐状态累积发散, 最终 policy 输出极端值.
        // robot_mujoco 的关节速度观测器经过 RL 训练匹配, 不能用简单指数滤波替代.
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
        // 无命令时: 零力矩, 让机器人自然下落
        if (model_ && data_) {
            for (int i = 0; i < model_->nu; i++) {
                data_->ctrl[i] = 0.0;
            }
        }
        return;
    }

    const auto& cmd = latest_cmd_;

    // 检查 PD 是否激活 (任一 kp 非零即为激活)
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

    // PD 控制: 匹配 UE 内部模式 (ApplyUdpControl)
    // tau = kp * (q_des - q) - kd * qvel
    // 无 qfrc_bias 重力补偿, 无扭矩限幅 — 与 UE 内部模式完全一致.
    // mc_ctrl (RL policy) 已包含 tau_ff 和重力补偿.
    //
    // 协议索引 → MuJoCo 关节映射 (与 UE ApplyUdpControl 完全一致):
    //   protoIdx = jointType * 4 + leg
    //   mjIdx    = leg * 3 + jointType
    //   jntIdx   = mjIdx + 1  (skip freejoint)

    for (int leg = 0; leg < 4; leg++) {
        for (int jointType = 0; jointType < 3; jointType++) {
            int mjIdx   = leg * 3 + jointType;   // MuJoCo actuator index
            int protoIdx = jointType * 4 + leg;   // Protocol array index
            int jntIdx  = mjIdx + 1;              // skip freejoint

            if (mjIdx >= model_->nu) break;

            double q  = data_->qpos[model_->jnt_qposadr[jntIdx]];
            double qd = data_->qvel[model_->jnt_dofadr[jntIdx]];

            // 从 cmd 读取 target, gain (按 jointType 选择正确的字段)
            double target = 0, kp = 0, kd = 0, tau_ff = 0;
            double qd_des = 0;

            if (jointType == 0) { // ABAD
                if (cmd.q_des_abad_size() > leg)  target = cmd.q_des_abad(leg);
                if (cmd.qd_des_abad_size() > leg) qd_des = cmd.qd_des_abad(leg);
                if (cmd.kp_abad_size() > leg)     kp = cmd.kp_abad(leg);
                if (cmd.kd_abad_size() > leg)     kd = cmd.kd_abad(leg);
                if (cmd.tau_abad_ff_size() > leg) tau_ff = cmd.tau_abad_ff(leg);
            } else if (jointType == 1) { // HIP
                if (cmd.q_des_hip_size() > leg)   target = cmd.q_des_hip(leg);
                if (cmd.qd_des_hip_size() > leg)  qd_des = cmd.qd_des_hip(leg);
                if (cmd.kp_hip_size() > leg)      kp = cmd.kp_hip(leg);
                if (cmd.kd_hip_size() > leg)      kd = cmd.kd_hip(leg);
                if (cmd.tau_hip_ff_size() > leg)  tau_ff = cmd.tau_hip_ff(leg);
            } else { // KNEE
                if (cmd.q_des_knee_size() > leg)  target = cmd.q_des_knee(leg);
                if (cmd.qd_des_knee_size() > leg) qd_des = cmd.qd_des_knee(leg);
                if (cmd.kp_knee_size() > leg)     kp = cmd.kp_knee(leg);
                if (cmd.kd_knee_size() > leg)     kd = cmd.kd_knee(leg);
                if (cmd.tau_knee_ff_size() > leg) tau_ff = cmd.tau_knee_ff(leg);
            }

            // 匹配 UE: tau = kp*(target-q) - kd*qvel + tau_ff
            data_->ctrl[mjIdx] = kp * (target - q) - kd * qd + tau_ff;
        }
    }

    // 诊断日志: 每100步输出完整控制信息
    static FILE* diag_fp = nullptr;
    if (!diag_fp) {
        diag_fp = fopen("/tmp/mujoco_diag.log", "w");
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
        // FR joint details (leg=0, ctrl_idx=0)
        for (int j = 0; j < 3; j++) {
            int qi = 7 + j, vi = 6 + j, ci = j;  // ci = ctrl 索引 (FR: 0,1,2)
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
                    data_->qpos[qi], qdes, data_->qvel[vi], data_->ctrl[ci], data_->qfrc_bias[vi]);
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
