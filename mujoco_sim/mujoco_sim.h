/**
 * MujocoSim - 独立 MuJoCo 物理仿真服务器
 *
 * 替代 Matrix 的 robot_mujoco 预编译二进制。
 * 架构参考 Matrix ZsibotSdkBridgeThread，实现物理/通信分离：
 *
 * 数据流：
 *   mc_ctrl ──eCAL [mujoco_cmd]──→ CarlaSdkBridgeThread ──shared_state──→ PhysicsThread
 *                                                                              │
 *                                        mc_ctrl ←──eCAL [mujoco_state]──←─────┤
 *                                        CarlaUE5 ←──UDP 9999────────────←─────┘
 *
 * 线程模型：
 *   PhysicsThread (500Hz):     ApplyControl → mj_step → 写入 SharedState
 *   CarlaSdkBridgeThread:      读取 SharedState → eCAL 发布 (500Hz) + UDP 发送 (~100Hz)
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
#include <vector>

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
    int         udp_target_port  = 9999;   // CarlaUnreal 监听此端口 (与 Matrix 一致)
    bool        enable_udp       = true;
    bool        enable_ecal      = true;

    // 仿真参数
    double      sim_rate_hz      = 500.0;  // 物理步进频率
    double      publish_rate_hz  = 500.0;  // eCAL 状态发布频率
    double      udp_rate_hz      = 100.0;  // UDP 渲染状态发送频率 (与 Matrix ZsibotSdkBridge 一致)
    bool        enable_gui       = true;   // GLFW 可视化窗口

    // 从 config.yaml 加载
    static SimConfig LoadFromFile(const std::string& path);
};

// ==================== 线程间共享状态 ====================

/**
 * @brief PhysicsThread 和 CarlaSdkBridgeThread 之间的共享数据
 *
 * PhysicsThread 每步写入 (mj_step 之后)
 * CarlaSdkBridgeThread 每帧读取 (构建 RobotState 发送)
 */
struct SharedState {
    // 关节位置 [12]: qpos[7:18]
    std::vector<float> q_joint;      // 12 个关节位置

    // 关节速度 [12]: qvel[6:17]
    std::vector<float> qd_joint;     // 12 个关节速度

    // 关节力矩 [12]: sensordata[24:36]
    std::vector<float> tau_joint;    // 12 个关节力矩反馈

    // Base position [3]: sensordata[45:47] (framepos sensor)
    std::vector<float> base_position;

    // Base quaternion [4]: sensordata[36:40] (framequat/imu sensor, w,x,y,z)
    std::vector<float> base_quat;

    // 陀螺仪 [3]: sensordata[39:42]
    std::vector<float> imu_gyro;

    // 加速度计 [3]: sensordata[42:45]
    std::vector<float> imu_acc;

    // 线速度 [3]: sensordata[48:51] (framelinvel)
    std::vector<float> base_linvel;

    // ---- 原始 MuJoCo 数据 (用于 UDP 9999 渲染同步) ----
    double raw_qpos[19] = {};   // 完整 qpos: base_pos[3] + base_quat[4] + joints[12]
    double raw_qvel[18] = {};   // 完整 qvel: base_vel[3] + base_angvel[3] + joints[12]
    double raw_tau[12]  = {};   // 电机扭矩 (从 sensordata 读取)

    // 仿真时间
    double sim_time = 0.0;

    // 写入时间戳 (用于 Bridge 判断数据新鲜度)
    uint64_t write_seq = 0;
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

    // 状态构建 (由 CarlaSdkBridgeThread 调用, 调用者必须持有 state_mutex_)
    robot_sdk::pb::RobotState BuildRobotState();

    // eCAL 回调 (mc_ctrl → mujoco_sim)
    void OnRobotCmdReceived(const char* topic_name,
                            const robot_sdk::pb::RobotCmd& msg,
                            long long time, long long clock, long long id);

    // UDP 发送
    bool InitUdp();
    void SendUdp(const std::string& data);

    // 打印模型信息
    void PrintModelInfo();

    // ---- 线程 ----

    /**
     * @brief 物理仿真线程 (500Hz)
     * ApplyControl → mj_step → 写入 SharedState
     */
    void PhysicsThreadFunc();

    /**
     * @brief 通信桥接线程 (参考 Matrix ZsibotSdkBridgeThread)
     * 读取 SharedState → eCAL 发布 + UDP 发送给 CarlaUE5
     */
    void CarlaSdkBridgeThreadFunc();

    /**
     * @brief 将当前 MuJoCo data_ 写入 SharedState (物理线程调用)
     */
    void WriteSharedState();

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

    // ---- 线程间共享 ----

    // 物理线程写入, Bridge 线程读取
    std::mutex state_mutex_;
    SharedState shared_state_;

    // 控制命令 (eCAL 回调写入, 物理线程读取)
    std::mutex cmd_mutex_;
    robot_sdk::pb::RobotCmd latest_cmd_;
    bool has_cmd_ = false;

    // 线程对象
    std::thread physics_thread_;
    std::thread bridge_thread_;

    // eCAL
    std::unique_ptr<eCAL::protobuf::CPublisher<robot_sdk::pb::RobotState>> ecal_pub_;
    std::unique_ptr<eCAL::protobuf::CSubscriber<robot_sdk::pb::RobotCmd>>  ecal_sub_;

    // UDP
    int udp_sock_ = -1;
    struct sockaddr_in udp_dest_;

    // 统计
    uint64_t step_count_ = 0;
    uint64_t publish_count_ = 0;
    uint64_t udp_send_count_ = 0;

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
