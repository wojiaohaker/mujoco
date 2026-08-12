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
import threading

try:
    from pynput import keyboard as pynput_keyboard
    HAS_PYNPUT = True
except ImportError:
    HAS_PYNPUT = False

# ==================== 配置 ====================

# ONNX 模型路径
DEFAULT_ONNX_PATH = os.path.expanduser(
    "~/Softwares/IsaacLab/logs/rsl_rl/xgb_flat/2026-08-12_09-55-03/exported/policy.onnx"
)

# MuJoCo 模型路径
MJCF_PATH = os.path.expanduser(
    "~/Softwares/Matrix/src/robot_mujoco/zsibot_robots/xgb/scene.xml"
)

# PD 控制参数 (对齐 Matrix)
KP_RL = 20.0       # RL 模式 Kp
KD_RL = 0.7        # RL 模式 Kd
KP_STANDUP = 150.0  # 站立模式 Kp (用户要求: 150)
KD_STANDUP = 2.0    # 站立模式 Kd (用户要求: 2.0)
KP_BALANCE = 300.0  # BALANCE 模式高增益 Kp (确保能克服重力)
KD_BALANCE = 5.0    # BALANCE 模式高增益 Kd
STANDUP_GAIN_RAMP_DURATION = 1.0  # 增益渐进时间 (用户要求: 1.0s)

# 默认关节位置 (对齐 Matrix)
# 站立姿态: ABAD=0, HIP=0.8, KNEE=-1.5
STAND_JOINT_POS = np.array([
    0.0,  0.8, -1.5,  # FAR: ABAD, HIP, KNEE
    0.0,  0.8, -1.5,  # FBL
    0.0,  0.8, -1.5,  # RAR
    0.0,  0.8, -1.5,  # RBL
])

# 趴下姿态: ABAD=0, HIP=1.4, KNEE=-2.4
LIE_JOINT_POS = np.array([
    0.0,  1.4, -2.4,
    0.0,  1.4, -2.4,
    0.0,  1.4, -2.4,
    0.0,  1.4, -2.4,
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
        print(f"     物理步长: {self.model.opt.timestep:.4f}s ({1/self.model.opt.timestep:.0f}Hz)")

        # 初始化状态
        self.last_actions = np.zeros(12)
        self.control_time = 0.0
        self._gravity_comp = np.zeros(12)  # 重力补偿力矩

        # FSM 状态: 'PASSIVE' -> 'STANDUP' -> 'BALANCE' -> 'RL_MIX'
        self.fsm_state = 'PASSIVE'
        self.standup_start_time = None
        self.standup_duration = 3.0  # 站立过渡时间 (用户要求: 3s)
        self.balance_duration = 3.0  # 站立后等待时间 (用户要求: 3s)
        self._standup_gain = 0.0     # 站立增益渐进系数

        # 按键状态
        self.key_standup = False   # U 键：站立
        self.key_liedown = False   # Space 键：趴下

        # PD 目标位置
        self.target_pos = LIE_JOINT_POS.copy()  # 初始目标：趴下姿态

        # 设置初始关节位置 (趴下姿态，身体贴近地面)
        self._reset_joints()

    def _reset_joints(self):
        """重置关节到趴下位置，身体贴近地面"""
        # qpos: [base_pos(3), base_quat(4), joints(12)]
        self.data.qpos[7:19] = LIE_JOINT_POS
        # 趴下时身体贴近地面 (约 0.12m)
        self.data.qpos[2] = 0.12
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
        - base_lin_vel: 3      (基座线速度, 本体坐标系)
        - base_ang_vel: 3      (基座角速度, 本体坐标系)
        - projected_gravity: 3 (重力投影, 本体坐标系)
        - velocity_commands: 3 (速度命令)
        - joint_pos: 12        (关节位置, Isaac Lab 顺序)
        - joint_vel: 12        (关节速度, Isaac Lab 顺序)
        - last_actions: 12     (上一步动作, Isaac Lab 顺序)
        """
        quat = self.data.qpos[3:7].copy()  # w, x, y, z

        # 基座线速度 (本体坐标系)
        # qvel[0:3] 是世界坐标系，需要转换
        base_lin_vel_world = self.data.qvel[0:3].copy()
        base_lin_vel = self._world_to_body(base_lin_vel_world, quat)

        # 基座角速度 (本体坐标系)
        base_ang_vel_world = self.data.qvel[3:6].copy()
        base_ang_vel = self._world_to_body(base_ang_vel_world, quat)

        # 重力投影 (本体坐标系)
        gravity_world = np.array([0, 0, -1])
        projected_gravity = self._world_to_body(gravity_world, quat)

        # 关节位置和速度 (MuJoCo 顺序 -> Isaac Lab 顺序)
        # Isaac Lab 使用 joint_pos_rel (相对默认姿态的偏差)
        joint_pos_mj = self._get_joint_pos_mujoco()
        joint_vel_mj = self._get_joint_vel_mujoco()

        joint_pos_isaac = joint_pos_mj[MUJOCO_TO_ISAAC]
        joint_vel_isaac = joint_vel_mj[MUJOCO_TO_ISAAC]

        # 转换为相对默认姿态的偏差 (Isaac Lab 的 joint_pos_rel)
        joint_pos_rel = joint_pos_isaac - STAND_JOINT_POS
        # joint_vel_rel = joint_vel - 0 = joint_vel (默认速度为0)

        # 构造观测
        obs = np.concatenate([
            base_lin_vel,       # 3
            base_ang_vel,       # 3
            projected_gravity,  # 3
            VELOCITY_CMD,       # 3
            joint_pos_rel,      # 12 (相对偏差)
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

    def _apply_pd_control(self, kp=None, kd=None):
        """
        在每个物理步执行 PD 控制
        τ = Kp * (target - q) - Kd * q̇ + gravity_compensation
        
        gravity_comp 由每个控制步开始时计算，这里直接复用。
        """
        if kp is None:
            kp = KP_RL
        if kd is None:
            kd = KD_RL

        current_pos = self.data.qpos[7:19]
        current_vel = self.data.qvel[6:18]

        # PD 控制 + 重力补偿（每个控制步计算一次，物理子步复用）
        tau = kp * (self.target_pos - current_pos) - kd * current_vel
        tau += self._gravity_comp

        # 限幅 ±28 Nm
        tau = np.clip(tau, -28.0, 28.0)

        # 应用到执行器
        self.data.ctrl[:] = tau

    def _compute_gravity_compensation(self):
        """
        在每个控制步开始时计算重力补偿力矩。
        使用 mj_inverse: 设 qacc=0, 返回维持当前姿态所需的力矩 = C(q,qdot) + g(q)
        
        注意：只在 RL_MIX 阶段调用，避免在不稳定姿态下计算错误补偿。
        """
        if self.fsm_state != 'RL_MIX':
            # 非 RL 阶段不计算重力补偿
            return

        # 保存可能被 mj_inverse 修改的状态
        qacc_save = self.data.qacc.copy()
        qfrc_inverse_save = self.data.qfrc_inverse.copy()

        # 设目标加速度为 0
        self.data.qacc[:] = 0.0

        # 逆动力学：qfrc_inverse = M*qacc + C(q,qdot) + g(q)
        # 当 qacc=0 时: qfrc_inverse = C(q,qdot) + g(q)
        mujoco.mj_inverse(self.model, self.data)

        # 保存关节部分的重力补偿力矩
        self._gravity_comp = self.data.qfrc_inverse[6:18].copy()

        # 恢复状态（避免影响后续 mj_step）
        self.data.qacc[:] = qacc_save
        self.data.qfrc_inverse[:] = qfrc_inverse_save

    def _update_fsm(self):
        """FSM 状态转换"""
        if self.fsm_state == 'PASSIVE' and self.key_standup:
            # 按 U 键开始站立
            self.fsm_state = 'STANDUP'
            self.standup_start_time = self.data.time
            self.target_pos = STAND_JOINT_POS.copy()
            self.key_standup = False
            print(f"\n[FSM] PASSIVE -> STANDUP (t={self.data.time:.1f}s)")
            print(f"      目标关节: {self.target_pos}")

        elif self.fsm_state == 'STANDUP':
            # 站立过渡完成，进入 BALANCE 等待
            if self.data.time - self.standup_start_time >= self.standup_duration:
                self.fsm_state = 'BALANCE'
                self.balance_start_time = self.data.time
                print(f"\n[FSM] STANDUP -> BALANCE (t={self.data.time:.1f}s)")
                print(f"      保持站立 {self.balance_duration}s，稳定后再启动 ONNX")

        elif self.fsm_state == 'BALANCE':
            # 等待完成后，切换到 RL 控制
            if self.data.time - self.balance_start_time >= self.balance_duration:
                self.fsm_state = 'RL_MIX'
                print(f"\n[FSM] BALANCE -> RL_MIX (t={self.data.time:.1f}s)")
                print(f"      开始 ONNX 策略控制")

        elif self.fsm_state == 'RL_MIX' and self.key_liedown:
            # 按 Space 键趴下
            self.fsm_state = 'STANDUP'  # 先回到 STANDUP 过渡
            self.standup_start_time = self.data.time
            self.target_pos = LIE_JOINT_POS.copy()  # 目标改为趴下
            self.key_liedown = False
            print(f"\n[FSM] RL_MIX -> STANDUP (趴下过渡) (t={self.data.time:.1f}s)")

    def _interpolate_target(self):
        """站立过渡：平滑插值关节目标 + 增益渐进"""
        if self.fsm_state != 'STANDUP':
            return

        # 计算插值进度 (0~1)
        elapsed = self.data.time - self.standup_start_time
        progress = min(elapsed / self.standup_duration, 1.0)

        # 平滑插值 (ease-in-out)
        t = progress * progress * (3 - 2 * progress)

        # 从趴下姿态插值到站立姿态
        self.target_pos = LIE_JOINT_POS + t * (STAND_JOINT_POS - LIE_JOINT_POS)

        # 增益渐进：前 1 秒从 0 线性增长到 1.0
        gain_progress = min(elapsed / STANDUP_GAIN_RAMP_DURATION, 1.0)
        self._standup_gain = gain_progress

    def run_step(self):
        """运行一步控制"""
        # FSM 状态转换
        self._update_fsm()

        if self.fsm_state == 'STANDUP':
            # 站立过渡：插值目标位置
            self._interpolate_target()
            return None, None

        elif self.fsm_state == 'BALANCE':
            # 保持站立姿态，PD 稳定
            self.target_pos = STAND_JOINT_POS.copy()
            return None, None

        elif self.fsm_state == 'RL_MIX':
            # ONNX 策略控制
            obs = self._get_observation()

            input_name = self.session.get_inputs()[0].name
            output_name = self.session.get_outputs()[0].name
            actions = self.session.run([output_name], {input_name: obs.reshape(1, -1)})[0][0]

            # 转换到 MuJoCo 顺序，更新目标位置
            actions_mj = actions[ISAAC_TO_MUJOCO]
            self.target_pos = STAND_JOINT_POS + actions_mj
            self.last_actions = actions.copy()

            return obs, actions

        return None, None

    def run(self):
        """运行仿真循环 (FSM: PASSIVE -> STANDUP -> RL_MIX)"""
        print("\n[INFO] 启动 MuJoCo 仿真")
        print("      FSM 状态: PASSIVE (趴着)")
        print("\n[控制说明]")
        print("      U 键:     站立 (PASSIVE -> STANDUP -> RL_MIX)")
        print("      Space 键: 趴下 (RL_MIX -> STANDUP -> PASSIVE)")
        print("      鼠标左键拖拽: 旋转视角")
        print("      鼠标右键拖拽: 平移视角")
        print("      滚轮:         缩放")
        print("      关闭窗口:     退出\n")

        # 打印初始状态
        base_pos = self.data.qpos[0:3]
        joint_pos = self.data.qpos[7:19]
        print(f"[初始状态]")
        print(f"  基座位置: ({base_pos[0]:.3f}, {base_pos[1]:.3f}, {base_pos[2]:.3f})")
        print(f"  关节位置: {joint_pos}")
        print(f"  趴下姿态: {LIE_JOINT_POS}")
        print(f"  站立姿态: {STAND_JOINT_POS}")

        with mujoco.viewer.launch_passive(self.model, self.data) as viewer:
            # 禁用 MuJoCo UI（防止快捷键冲突）
            viewer._render_ui = False

            # 全局键盘监听（不依赖窗口焦点）
            if HAS_PYNPUT:
                def on_press(key):
                    try:
                        if key.char == 'u':
                            self.key_standup = True
                        elif key.char == ' ':
                            self.key_liedown = True
                    except AttributeError:
                        if key == pynput_keyboard.Key.space:
                            self.key_liedown = True
                listener = pynput_keyboard.Listener(on_press=on_press)
                listener.start()
                print("[INFO] 全局键盘监听已启动 (pynput)")
            else:
                listener = None
                print("[WARN] pynput 未安装，无法使用键盘控制")
                print("       安装: pip install pynput")

            step = 0
            while viewer.is_running():
                start_time = time.time()

                # 控制步
                # 每个控制步开始时计算重力补偿（仅 RL_MIX 阶段）
                self._compute_gravity_compensation()

                obs, actions = self.run_step()

                # 物理步：每步都执行 PD 控制
                while self.data.time < self.control_time + CONTROL_DT:
                    # 根据 FSM 状态选择 PD 增益
                    if self.fsm_state == 'STANDUP':
                        # 增益渐进：从 0 到 1.0
                        kp = KP_STANDUP * self._standup_gain
                        kd = KD_STANDUP * self._standup_gain
                        self._apply_pd_control(kp, kd)
                    elif self.fsm_state == 'BALANCE':
                        # BALANCE 状态：用超高增益 PD 保持站立（无神经网络辅助）
                        self._apply_pd_control(KP_BALANCE, KD_BALANCE)
                    else:
                        self._apply_pd_control()
                    mujoco.mj_step(self.model, self.data)

                self.control_time = self.data.time

                # 更新可视化
                viewer.sync()

                # 打印状态 (每 50 步)
                step += 1
                if step % 50 == 0:
                    base_pos = self.data.qpos[0:3]
                    print(f"[Step {step}] FSM={self.fsm_state} "
                          f"pos=({base_pos[0]:.2f}, {base_pos[1]:.2f}, {base_pos[2]:.2f})")

                # 控制循环频率
                elapsed = time.time() - start_time
                if elapsed < CONTROL_DT:
                    time.sleep(CONTROL_DT - elapsed)

        print("\n[INFO] 仿真结束")

        # 清理键盘监听
        if HAS_PYNPUT and listener is not None:
            listener.stop()


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
