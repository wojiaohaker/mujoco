/**
 * MujocoSim - 独立 MuJoCo 物理仿真服务器
 *
 * 替代 Matrix 的 robot_mujoco 预编译二进制。
 * 功能：
 *   1. 加载 xgb 四足机器人模型，运行 mj_step() 物理仿真 (500Hz)
 *   2. eCAL 通信：发布 mujoco_state (RobotState)，订阅 mujoco_cmd (RobotCmd)
 *   3. UDP 25001 输出：将 RobotState 序列化后通过 UDP 发送给 CarlaUE5
 *   4. PD 控制律：tau = kp*(q_des-q) + kd*(qd_des-qd) + tau_ff
 *
 * 数据流：
 *   mc_ctrl ──eCAL [mujoco_cmd]──→ MujocoSim ──eCAL [mujoco_state]──→ mc_ctrl
 *                                      │
 *                                      └──UDP 25001──→ CarlaUE5
 */

#pragma once

#include <mujoco/mujoco.h>
#include <GLFW/glfw3.h>

#include <ecal/ecal.h>
#include <ecal/msg/protobuf/publisher.h>
#include <ecal/msg/protobuf/subscriber.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>

// protobuf (系统安装: /usr/include/robot_sdk.pb.h)
#include <robot_sdk.pb.h>

namespace mujoco_sim {

// ==================== 配置 ====================
struct SimConfig {
    // 模型路径
    std::string robot_model_dir;    // 机器人模型目录 (含 xgb.xml)
    std::string scene_file;         // 场景文件名 (如 scene_terrain_yard.xml)

    // 通信
    std::string ecal_state_topic = "mujoco_state";
    std::string ecal_cmd_topic   = "mujoco_cmd";
    std::string udp_target_ip    = "127.0.0.1";
    int         udp_target_port  = 25001;  // CarlaUnreal 监听此端口
    bool        enable_udp       = true;
    bool        enable_ecal      = true;

    // 仿真参数
    double      sim_rate_hz      = 500.0;  // 物理步进频率
    double      publish_rate_hz  = 500.0;  // 状态发布频率
    bool        enable_gui       = true;   // GLFW 可视化窗口

    // 从 config.yaml 加载
    static SimConfig LoadFromFile(const std::string& path);
};

// ==================== 仿真器 ====================
class MujocoSim {
public:
    MujocoSim();
    ~MujocoSim();

    // 初始化（加载模型、创建通信）
    bool Initialize(const SimConfig& config);

    // 运行主循环（阻塞，直到 Stop() 被调用或窗口关闭）
    void Run();

    // 获取 model/data 指针（GUI 渲染用）
    mjModel* GetModel() const { return model_; }
    mjData*  GetData()  const { return data_; }

    // 停止
    void Stop();

    // 状态查询
    bool IsRunning() const { return running_.load(); }

private:
    // 物理仿真
    bool LoadModel(const SimConfig& config);
    void PhysicsStep();
    void ApplyControl();

    // 状态发布
    void PublishState();
    robot_sdk::pb::RobotState BuildRobotState();

    // eCAL 回调
    void OnRobotCmdReceived(const char* topic_name,
                            const robot_sdk::pb::RobotCmd& msg,
                            long long time, long long clock, long long id);

    // UDP 发送
    bool InitUdp();
    void SendUdp(const std::string& data);

    // 打印模型信息
    void PrintModelInfo();

    // GUI 渲染
    void RunWithGui();
    void RunHeadless();
    static void GuiKeyboard(GLFWwindow* window, int key, int scancode, int act, int mods);
    static void GuiMouseButton(GLFWwindow* window, int button, int act, int mods);
    static void GuiMouseMove(GLFWwindow* window, double xpos, double ypos);
    static void GuiScroll(GLFWwindow* window, double xoffset, double yoffset);

    // MuJoCo
    mjModel* model_ = nullptr;
    mjData*  data_  = nullptr;

    // 配置
    SimConfig config_;

    // 运行状态
    std::atomic<bool> running_{false};

    // 控制命令（线程安全）
    std::mutex cmd_mutex_;
    robot_sdk::pb::RobotCmd latest_cmd_;
    bool has_cmd_ = false;

    // eCAL
    std::unique_ptr<eCAL::protobuf::CPublisher<robot_sdk::pb::RobotState>> ecal_pub_;
    std::unique_ptr<eCAL::protobuf::CSubscriber<robot_sdk::pb::RobotCmd>>  ecal_sub_;

    // UDP
    int udp_sock_ = -1;
    struct sockaddr_in udp_dest_;        // CarlaUnreal (25001)

    // 统计
    uint64_t step_count_ = 0;
    uint64_t publish_count_ = 0;

    // GUI 状态（静态回调需要访问）
    static mjModel* s_model_;
    static mjData*  s_data_;
    static mjvCamera s_cam_;
    static mjvOption s_opt_;
    static mjvScene  s_scn_;
    static mjrContext s_con_;
    static bool s_button_left_;
    static bool s_button_middle_;
    static bool s_button_right_;
    static double s_lastx_;
    static double s_lasty_;
};

}  // namespace mujoco_sim
