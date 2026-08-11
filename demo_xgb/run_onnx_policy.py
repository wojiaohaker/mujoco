#!/usr/bin/env python3
"""
XGB 四足机器人 ONNX 策略推理 + MuJoCo 仿真

使用 Isaac Lab 训练导出的 policy.onnx 控制 xgb 机器人。

用法:
    python run_onnx_policy.py [onnx_path]

默认 ONNX 路径:
    ~/Softwares/IsaacLab/logs/rsl_rl/xgb_flat/2026-08-11_17-02-19/exported/policy.onnx
"""

import numpy as np
import mujoco
import mujoco.viewer
import onnxruntime as ort
import time
import sys
import os

# ==================== 配置 ====================

# ONNX 模型路径
DEFAULT_ONNX_PATH = os.path.expanduser(
    "~/Softwares/IsaacLab/logs/rsl_rl/xgb_flat/2026-08-11_17-02-19/exported/policy.onnx"
)

# MuJoCo 模型路径
MJCF_PATH = os.path.expanduser(
    "~/Softwares/Matrix/src/robot_mujoco/zsibot_robots/xgb/scene.xml"
)

# PD 控制参数 (对齐 Matrix: Kp=20, Kd=0.7)
KP = 20.0
KD = 0.7

# 默认关节位置 (对齐 Matrix: ABAD=0, HIP=0.8, KNEE=-1.5)
DEFAULT_JOINT_POS = np.array([
    0.0,  0.8, -1.5,  # FAR: ABAD, HIP, KNEE
    0.0,  0.8, -1.5,  # FBL: ABAD, HIP, KNEE
    0.0,  0.8, -1.5,  # RAR: ABAD, HIP, KNEE
    0.0,  0.8, -1.5,  # RBL: ABAD, HIP, KNEE
])

# 速度命令 (固定: 向前 0.5 m/s)
VELOCITY_CMD = np.array([0.5, 0.0, 0.0])  # vx, vy, wz

# 控制频率
CONTROL_DT = 0.02  # 50Hz (与 Isaac Lab 一致)

# ==================== 关节顺序映射 ====================

# Isaac Lab 动作/观测中的关节顺序 (从 play 输出):
# [FAR_ABAD, FBL_ABAD, RAR_ABAD, RBL_ABAD,
#  FAR_HIP, FBL_HIP, RAR_HIP, RBL_HIP,
#  FAR_KNEE, FBL_KNEE, RAR_KNEE, RBL_KNEE]

# MuJoCo actuator 顺序 (从 xgb.xml):
# [FAR_ABAD, FAR_HIP, FAR_KNEE,
#  FBL_ABAD, FBL_HIP, FBL_KNEE,
#  RAR_ABAD, RAR_HIP, RAR_KNEE,
#  RBL_ABAD, RBL_HIP, RBL_KNEE]

# Isaac Lab -> MuJoCo 映射
ISAAC_TO_MUJOCO = np.array([
    0,   # FAR_ABAD  -> mj[0]
    3,   # FBL_ABAD  -> mj[3]
    6,   # RAR_ABAD  -> mj[6]
    9,   # RBL_ABAD  -> mj[9]
    1,   # FAR_HIP   -> mj[1]
    4,   # FBL_HIP   -> mj[4]
    7,   # RAR_HIP   -> mj[7]
    10,  # RBL_HIP   -> mj[10]
    2,   # FAR_KNEE  -> mj[2]
    5,   # FBL_KNEE  -> mj[5]
    8,   # RAR_KNEE  -> mj[8]
    11,  # RBL_KNEE  -> mj[11]
])

# MuJoCo -> Isaac Lab 映射 (反向)
MUJOCO_TO_ISAAC = np.array([
    0,   # mj[0] FAR_ABAD  -> isaac[0]
    4,   # mj[1] FAR_HIP   -> isaac[4]
    8,   # mj[2] FAR_KNEE  -> isaac[8]
    1,   # mj[3] FBL_ABAD  -> isaac[1]
    5,   # mj[4] FBL_HIP   -> isaac[5]
    9,   # mj[5] FBL_KNEE  -> isaac[9]
    2,   # mj[6] RAR_ABAD  -> isaac[2]
    6,   # mj[7] RAR_HIP   -> isaac[6]
    10,  # mj[8] RAR_KNEE  -> isaac[10]
    3,   # mj[9] RBL_ABAD  -> isaac[3]
    7,   # mj[10] RBL_HIP  -> isaac[7]
    11,  # mj[11] RBL_KNEE -> isaac[11]
])


class XgbPolicyRunner:
    """XGB 机器人 ONNX 策略推理器"""

    def __init__(self, onnx_path: str):
        # 加载 ONNX 模型
        print(f"[INFO] 加载 ONNX 模型: {onnx_path}")
        # 使用 CPU (CUDA 可能不兼容当前环境)
        self.session = ort.InferenceSession(onnx_path, providers=['CPUExecutionProvider'])
        print(f"[OK] ONNX 模型加载成功")
        print(f"     输入: {self.session.get_inputs()[0].name} {self.session.get_inputs()[0].shape}")
        print(f"     输出: {self.session.get_outputs()[0].name} {self.session.get_outputs()[0].shape}")

        # 加载 MuJoCo 模型
        print(f"\n[INFO] 加载 MuJoCo 模型: {MJCF_PATH}")
        self.model = mujoco.MjModel.from_xml_path(MJCF_PATH)
        self.data = mujoco.MjData(self.model)
        print(f"[OK] MuJoCo 模型加载成功")
        print(f"     关节数: {self.model.njnt}")
        print(f"     执行器数: {self.model.nu}")

        # 初始化状态
        self.last_actions = np.zeros(12)
        self.control_time = 0.0

        # 设置初始关节位置
        self._reset_joints()

    def _reset_joints(self):
        """重置关节到默认站立位置"""
        # qpos: [base_pos(3), base_quat(4), joints(12)]
        self.data.qpos[7:19] = DEFAULT_JOINT_POS
        mujoco.mj_forward(self.model, self.data)

    def _get_joint_pos_mujoco(self) -> np.ndarray:
        """获取 MuJoCo 关节位置 (12维)"""
        return self.data.qpos[7:19].copy()

    def _get_joint_vel_mujoco(self) -> np.ndarray:
        """获取 MuJoCo 关节速度 (12维)"""
        return self.data.qvel[6:18].copy()

    def _get_observation(self) -> np.ndarray:
        """
        构造 48 维观测 (Isaac Lab 格式)

        观测结构:
        - base_lin_vel: 3      (基座线速度, 世界坐标系)
        - base_ang_vel: 3      (基座角速度, 本体坐标系)
        - projected_gravity: 3 (重力投影, 本体坐标系)
        - velocity_commands: 3 (速度命令)
        - joint_pos: 12        (关节位置, Isaac Lab 顺序)
        - joint_vel: 12        (关节速度, Isaac Lab 顺序)
        - last_actions: 12     (上一步动作, Isaac Lab 顺序)
        """
        # 基座线速度 (世界坐标系)
        base_lin_vel = self.data.qvel[0:3].copy()

        # 基座角速度 (本体坐标系) - 从 IMU 传感器或 qvel 获取
        # qvel[3:6] 是基座角速度 (世界坐标系)
        # 需要转换到本体坐标系
        base_ang_vel_world = self.data.qvel[3:6].copy()
        # 使用四元数转换到本体坐标系
        quat = self.data.qpos[3:7].copy()  # w, x, y, z
        base_ang_vel = self._world_to_body(base_ang_vel_world, quat)

        # 重力投影 (本体坐标系)
        # MuJoCo 中重力方向是 -Z, 需要转换到本体坐标系
        gravity_world = np.array([0, 0, -1])
        projected_gravity = self._world_to_body(gravity_world, quat)

        # 关节位置和速度 (MuJoCo 顺序 -> Isaac Lab 顺序)
        joint_pos_mj = self._get_joint_pos_mujoco()
        joint_vel_mj = self._get_joint_vel_mujoco()

        joint_pos_isaac = joint_pos_mj[MUJOCO_TO_ISAAC]
        joint_vel_isaac = joint_vel_mj[MUJOCO_TO_ISAAC]

        # 构造观测
        obs = np.concatenate([
            base_lin_vel,       # 3
            base_ang_vel,       # 3
            projected_gravity,  # 3
            VELOCITY_CMD,       # 3
            joint_pos_isaac,    # 12
            joint_vel_isaac,    # 12
            self.last_actions,  # 12
        ])  # 总共 48

        return obs.astype(np.float32)

    def _world_to_body(self, vec_world: np.ndarray, quat: np.ndarray) -> np.ndarray:
        """将世界坐标系向量转换到本体坐标系"""
        # 四元数旋转: q^{-1} * v * q
        w, x, y, z = quat
        # 旋转矩阵 (从四元数)
        R = np.array([
            [1 - 2*(y*y + z*z), 2*(x*y - w*z), 2*(x*z + w*y)],
            [2*(x*y + w*z), 1 - 2*(x*x + z*z), 2*(y*z - w*x)],
            [2*(x*z - w*y), 2*(y*z + w*x), 1 - 2*(x*x + y*y)]
        ])
        return R.T @ vec_world

    def _apply_action(self, actions_isaac: np.ndarray):
        """
        应用动作 (PD 控制)

        actions_isaac: Isaac Lab 顺序的 12 维关节位置偏移
        """
        # 转换到 MuJoCo 顺序
        actions_mj = actions_isaac[ISAAC_TO_MUJOCO]

        # 目标位置 = 默认位置 + 偏移
        target_pos = DEFAULT_JOINT_POS + actions_mj

        # 当前关节状态
        current_pos = self._get_joint_pos_mujoco()
        current_vel = self._get_joint_vel_mujoco()

        # PD 控制: tau = Kp * (target - current) - Kd * vel
        tau = KP * (target_pos - current_pos) - KD * current_vel

        # 限幅 ±28 Nm
        tau = np.clip(tau, -28.0, 28.0)

        # 应用到执行器
        self.data.ctrl[:] = tau

        # 保存动作 (用于下一步观测)
        self.last_actions = actions_isaac.copy()

    def run_step(self):
        """运行一步控制"""
        # 构造观测
        obs = self._get_observation()

        # ONNX 推理
        input_name = self.session.get_inputs()[0].name
        output_name = self.session.get_outputs()[0].name
        actions = self.session.run([output_name], {input_name: obs.reshape(1, -1)})[0][0]

        # 应用动作
        self._apply_action(actions)

        return obs, actions

    def run(self):
        """运行仿真循环"""
        print("\n[INFO] 启动 MuJoCo 仿真...")
        print(f"      控制频率: {1/CONTROL_DT:.0f} Hz")
        print(f"      速度命令: vx={VELOCITY_CMD[0]:.1f}, vy={VELOCITY_CMD[1]:.1f}, wz={VELOCITY_CMD[2]:.1f}")
        print("\n[控制说明]")
        print("      鼠标左键拖拽: 旋转视角")
        print("      鼠标右键拖拽: 平移视角")
        print("      滚轮:         缩放")
        print("      关闭窗口:     退出\n")

        with mujoco.viewer.launch_passive(self.model, self.data) as viewer:
            step = 0
            while viewer.is_running():
                start_time = time.time()

                # 运行控制
                obs, actions = self.run_step()

                # 步进仿真 (直到达到控制周期)
                while self.data.time < self.control_time + CONTROL_DT:
                    mujoco.mj_step(self.model, self.data)

                self.control_time = self.data.time

                # 更新可视化
                viewer.sync()

                # 打印状态 (每 50 步)
                step += 1
                if step % 50 == 0:
                    base_pos = self.data.qpos[0:3]
                    print(f"[Step {step}] "
                          f"pos=({base_pos[0]:.2f}, {base_pos[1]:.2f}, {base_pos[2]:.2f}) "
                          f"cmd=({VELOCITY_CMD[0]:.1f}, {VELOCITY_CMD[1]:.1f}, {VELOCITY_CMD[2]:.1f})")

                # 控制循环频率
                elapsed = time.time() - start_time
                if elapsed < CONTROL_DT:
                    time.sleep(CONTROL_DT - elapsed)

        print("\n[INFO] 仿真结束")


def main():
    # ONNX 路径
    onnx_path = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_ONNX_PATH

    if not os.path.exists(onnx_path):
        print(f"[ERROR] ONNX 模型不存在: {onnx_path}")
        sys.exit(1)

    # 运行策略
    runner = XgbPolicyRunner(onnx_path)
    runner.run()


if __name__ == "__main__":
    main()
