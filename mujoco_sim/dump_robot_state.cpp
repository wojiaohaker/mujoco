/**
 * 监听 mujoco_state eCAL topic，打印完整 RobotState 内容
 * 基于实际编译的 robot_sdk.pb.h 字段定义
 */
#include <ecal/ecal.h>
#include <ecal/msg/protobuf/subscriber.h>
#include <robot_sdk.pb.h>

#include <chrono>
#include <cstdio>
#include <thread>

static int g_count = 0;

template<typename T>
static void PrintArr(const char* name, const T& arr, int size) {
    printf("%s[%d]:", name, size);
    for (int i = 0; i < size; i++) printf(" %.6f", arr.Get(i));
    printf("\n");
}

static void PrintState(const robot_sdk::pb::RobotState& msg) {
    g_count++;
    if (g_count > 5 && g_count % 500 != 0) return;

    printf("\n=== RobotState #%d (size=%d bytes) ===\n", g_count, (int)msg.ByteSizeLong());

    PrintArr("q_abad   ", msg.q_abad(), msg.q_abad_size());
    PrintArr("q_hip    ", msg.q_hip(), msg.q_hip_size());
    PrintArr("q_knee   ", msg.q_knee(), msg.q_knee_size());
    PrintArr("q_foot   ", msg.q_foot(), msg.q_foot_size());
    PrintArr("qd_abad  ", msg.qd_abad(), msg.qd_abad_size());
    PrintArr("qd_hip   ", msg.qd_hip(), msg.qd_hip_size());
    PrintArr("qd_knee  ", msg.qd_knee(), msg.qd_knee_size());
    PrintArr("qd_foot  ", msg.qd_foot(), msg.qd_foot_size());
    PrintArr("tau_abad_fb", msg.tau_abad_fb(), msg.tau_abad_fb_size());
    PrintArr("tau_hip_fb ", msg.tau_hip_fb(), msg.tau_hip_fb_size());
    PrintArr("tau_knee_fb", msg.tau_knee_fb(), msg.tau_knee_fb_size());
    PrintArr("tau_foot_fb", msg.tau_foot_fb(), msg.tau_foot_fb_size());
    PrintArr("quat     ", msg.quat(), msg.quat_size());
    PrintArr("gyro     ", msg.gyro(), msg.gyro_size());
    PrintArr("acc      ", msg.acc(), msg.acc_size());
    PrintArr("rpy      ", msg.rpy(), msg.rpy_size());
    printf("time_stamp: %lu\n", (unsigned long)msg.time_stamp());
    PrintArr("depth    ", msg.depth(), msg.depth_size());
    PrintArr("position ", msg.position(), msg.position_size());
    PrintArr("v_world  ", msg.v_world(), msg.v_world_size());
    fflush(stdout);
}

int main(int argc, char* argv[]) {
    eCAL::Initialize(argc, argv, "dump_robot_state");
    eCAL::protobuf::CSubscriber<robot_sdk::pb::RobotState> sub("mujoco_state");
    sub.AddReceiveCallback(
        [](const char*, const robot_sdk::pb::RobotState& msg,
           long long, long long, long long) {
            PrintState(msg);
        });

    printf("[Dump] Listening on mujoco_state, Ctrl+C to exit...\n");
    for (int i = 0; i < 600 && eCAL::Ok(); i++) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    eCAL::Finalize();
    return 0;
}
