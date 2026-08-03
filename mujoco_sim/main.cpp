/**
 * mujoco_sim - 独立 MuJoCo 物理仿真服务器
 *
 * 替代 Matrix 的 robot_mujoco，与 mc_ctrl (type=5) 通过 eCAL 通信，
 * 同时通过 UDP 25001 向 CarlaUE5 发送 RobotState。
 *
 * 用法:
 *   ./mujoco_sim [config.yaml路径]
 *   默认: ../config.yaml (与 robot_mujoco 相同)
 */

#include "mujoco_sim.h"

#include <csignal>
#include <iostream>
#include <string>

// 全局仿真器指针（用于信号处理）
static mujoco_sim::MujocoSim* g_sim = nullptr;

void signal_handler(int sig) {
    std::cout << "\n[Signal] 收到信号 " << sig << ", 正在停止..." << std::endl;
    if (g_sim) {
        g_sim->Stop();
    }
}

int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "  mujoco_sim - MuJoCo Physics Server" << std::endl;
    std::cout << "  (替代 Matrix robot_mujoco)" << std::endl;
    std::cout << "========================================" << std::endl;

    // 配置文件路径
    std::string config_path = "../config.yaml";  // 默认（与 robot_mujoco 相同）
    if (argc >= 2) {
        config_path = argv[1];
    }

    // 加载配置
    auto config = mujoco_sim::SimConfig::LoadFromFile(config_path);

    // 如果 robot_model_dir 不是绝对路径，尝试相对路径解析
    if (!config.robot_model_dir.empty() && config.robot_model_dir[0] != '/') {
        // 相对于可执行文件目录
        std::string exe_dir = argv[0];
        auto pos = exe_dir.find_last_of('/');
        if (pos != std::string::npos) {
            exe_dir = exe_dir.substr(0, pos);
        } else {
            exe_dir = ".";
        }
        // robot_model_dir 可能是 "xgb"，需要找到完整路径
        // 默认使用 Matrix 的模型路径
        config.robot_model_dir =
            "/home/qiyuan/Softwares/Matrix/src/robot_mujoco/zsibot_robots/" +
            config.robot_model_dir;
    }

    // 创建仿真器
    mujoco_sim::MujocoSim sim;
    g_sim = &sim;

    // 注册信号处理
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    // 初始化
    if (!sim.Initialize(config)) {
        std::cerr << "[Main] 初始化失败!" << std::endl;
        return 1;
    }

    // 运行（阻塞）
    sim.Run();

    std::cout << "[Main] 退出" << std::endl;
    return 0;
}
