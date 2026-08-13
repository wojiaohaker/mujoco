#!/usr/bin/env python3
"""
Go2 四足机器人 ONNX 策略推理 + MuJoCo 仿真

使用 Isaac Lab 训练导出的 policy.onnx 控制 Go2 机器人。
用于验证收敛后的模型在 MuJoCo 中的表现。

用法:
    python run_go2_onnx_policy.py [onnx_path]
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

# ONNX 模型路径 (Go2 训练好的模型)
DEFAULT_ONNX_PATH = os.path.expanduser(
    "~/Softwares/IsaacLab/logs/rsl_rl/unitree_go2_flat/2026-08-12_16-06-07/exported/policy.onnx"
)

# MuJoCo 模型路径 (Go2 无轮子版本)
MJCF_PATH = os.path.expanduser(
    "~/Softwares/Matrix/src/robot_mujoco/zsibot_robots/go2/scene_terrain_flat.xml"
)

# PD 控制参数 (精确匹配 Isaac Lab Go2: stiffness=25, damping=0.5)
KP_RL = 25.0
KD_RL = 0.5
KP_STANDUP = 150.0
KD_STANDUP = 2.0
KP_BALANCE = 300.0
KD_BALANCE = 5.0
STANDUP_GAIN_RAMP_DURATION = 1.0

# Go2 默认关节姿态 (来自 Isaac Lab unitree.py)
# FL_hip=0.1, FR_hip=-0.1, thigh(F)=0.8, thigh(R)=1.0, calf=-1.5
# MuJoCo joint order (qpos[7:19]): FL→FR→RL→RR, each: hip,thigh,calf
STAND_JOINT_POS = np.array([
    0.1,  0.8, -1.5,   # FL: hip, thigh, calf
   -0.1,  0.8, -1.5,   # FR
    0.1,  1.0, -1.5,   # RL
   -0.1,  1.0, -1.5,   # RR
])

LIE_JOINT_POS = np.array([
    0.1,  1.4, -2.4,   # FL
   -0.1,  1.4, -2.4,   # FR
    0.1,  1.4, -2.4,   # RL
   -0.1,  1.4, -2.4,   # RR
])

# 速度命令
VELOCITY_CMD = np.array([0.0, 0.0, 0.0])

# 控制频率 (匹配 Isaac Lab: sim.dt=0.005 × decimation=4 = 0.02s = 50Hz)
CONTROL_DT = 0.02  # 50Hz (策略更新频率)

# ==================== 关节顺序映射 ====================

# Isaac Lab Go2 关节顺序 (按类型分组):
# [FL_hip, FR_hip, RL_hip, RR_hip,     ← hip×4
#  FL_thigh, FR_thigh, RL_thigh, RR_thigh, ← thigh×4
#  FL_calf, FR_calf, RL_calf, RR_calf]    ← calf×4
#
# MuJoCo qpos[7:19] 顺序 (按腿分组):
# [FL_hip, FL_thigh, FL_calf,   ← 前左腿
#  FR_hip, FR_thigh, FR_calf,   ← 前右腿
#  RL_hip, RL_thigh, RL_calf,   ← 后左腿
#  RR_hip, RR_thigh, RR_calf]   ← 后右腿

# MuJoCo → Isaac Lab
MUJOCO_TO_ISAAC = np.array([0, 3, 6, 9, 1, 4, 7, 10, 2, 5, 8, 11])
# Isaac Lab → MuJoCo
ISAAC_TO_MUJOCO = np.array([0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7, 11])

# MuJoCo actuator 顺序 → joint 顺序
# Actuators: [FR_hip(0), FR_thigh(1), FR_calf(2), FL_hip(3), FL_thigh(4), FL_calf(5),
#             RR_hip(6), RR_thigh(7), RR_calf(8), RL_hip(9), RL_thigh(10), RL_calf(11)]
# Joints:    [FL_hip(0), FL_thigh(1), FL_calf(2), FR_hip(3), FR_thigh(4), FR_calf(5),
#             RL_hip(6), RL_thigh(7), RL_calf(8), RR_hip(9), RR_thigh(10), RR_calf(11)]
# Actuator[i] → Joint[ACTUATOR_TO_JOINT[i]]
ACTUATOR_TO_JOINT = np.array([3, 4, 5, 0, 1, 2, 9, 10, 11, 6, 7, 8])
JOINT_TO_ACTUATOR = np.array([3, 4, 5, 0, 1, 2, 9, 10, 11, 6, 7, 8])  # symmetric!


class Go2PolicyRunner:
    """Go2 机器人 ONNX 策略推理器"""

    def __init__(self, onnx_path: str):
        # 加载 ONNX 模型
        print(f"[INFO] 加载 ONNX 模型: {onnx_path}")
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

        # 根据实际模型步长计算每控制步的物理子步数
        self.steps_per_control = max(1, round(CONTROL_DT / self.model.opt.timestep))
        print(f"     策略频率: {1/CONTROL_DT:.0f}Hz (每 {self.steps_per_control} 个物理步)")

        # 初始化状态
        self.last_actions = np.zeros(12)
        self.control_time = 0.0
        self._gravity_comp = np.zeros(12)

        # FSM
        self.fsm_state = 'PASSIVE'
        self.standup_start_time = None
        self.standup_duration = 3.0
        self._standup_gain = 0.0

        # 按键状态
        self.key_standup = False
        self.key_liedown = False
        self.key_w = False
        self.key_s = False
        self.key_a = False
        self.key_d = False
        self.key_q = False
        self.key_e = False

        # PD 目标
        self.target_pos = LIE_JOINT_POS.copy()

        # 设置初始姿态
        self._reset_joints()

    def _reset_joints(self):
        """重置关节到趴下位置"""
        self.data.qpos[7:19] = LIE_JOINT_POS
        self.data.qpos[0:3] = [0.0, 0.0, 0.12]
        self.data.qpos[3:7] = [1.0, 0.0, 0.0, 0.0]
        mujoco.mj_forward(self.model, self.data)

    def _get_joint_pos_mujoco(self) -> np.ndarray:
        """获取 MuJoCo 关节位置 (12维, joint order)"""
        return self.data.qpos[7:19].copy()

    def _get_joint_vel_mujoco(self) -> np.ndarray:
        """获取 MuJoCo 关节速度 (12维, joint order)"""
        return self.data.qvel[6:18].copy()

    def _get_observation(self) -> np.ndarray:
        """构造 48 维观测 (Isaac Lab 格式)"""
        quat = self.data.qpos[3:7].copy()

        # 基座速度 (本体坐标系)
        base_lin_vel = self._world_to_body(self.data.qvel[0:3].copy(), quat)
        base_ang_vel = self._world_to_body(self.data.qvel[3:6].copy(), quat)

        # 重力投影
        projected_gravity = self._world_to_body(np.array([0, 0, -1]), quat)

        # 关节 (MuJoCo joint order → Isaac Lab order)
        joint_pos_mj = self._get_joint_pos_mujoco()
        joint_vel_mj = self._get_joint_vel_mujoco()

        joint_pos_rel_mj = joint_pos_mj - STAND_JOINT_POS
        joint_pos_rel = joint_pos_rel_mj[MUJOCO_TO_ISAAC]
        joint_vel_isaac = joint_vel_mj[MUJOCO_TO_ISAAC]

        obs = np.concatenate([
            base_lin_vel,       # 3
            base_ang_vel,       # 3
            projected_gravity,  # 3
            VELOCITY_CMD,       # 3
            joint_pos_rel,      # 12
            joint_vel_isaac,    # 12
            self.last_actions,  # 12
        ])

        return obs.astype(np.float32)

    def _world_to_body(self, vec_world: np.ndarray, quat: np.ndarray) -> np.ndarray:
        """将世界坐标系向量转换到本体坐标系"""
        w, x, y, z = quat
        R = np.array([
            [1 - 2*(y*y + z*z), 2*(x*y - w*z), 2*(x*z + w*y)],
            [2*(x*y + w*z), 1 - 2*(x*x + z*z), 2*(y*z - w*x)],
            [2*(x*z - w*y), 2*(y*z + w*x), 1 - 2*(x*x + y*y)]
        ])
        return R.T @ vec_world

    def _apply_pd_control(self, kp=None, kd=None):
        """隐式 PD 控制 + 重力补偿 (匹配 Isaac Lab PhysX 隐式积分器)"""
        if kp is None:
            kp = KP_RL
        if kd is None:
            kd = KD_RL

        current_pos = self.data.qpos[7:19]   # joint order
        current_vel = self.data.qvel[6:18]   # joint order
        dt = self.model.opt.timestep  # 0.002s

        # 隐式 PD: tau = (kp*e - kd*v - kp*dt*v) / (1 + kd*dt + kp*dt²)
        # 匹配 PhysX 的隐式积分，避免显式 Euler 的振荡问题
        pos_error = self.target_pos - current_pos
        denom = 1.0 + kd * dt + kp * dt * dt
        tau_joint = (kp * pos_error - kd * current_vel - kp * dt * current_vel) / denom
        tau_joint += self._gravity_comp

        # 限幅 ±23.5 Nm (Go2 effort limit)
        tau_joint = np.clip(tau_joint, -23.5, 23.5)

        # 转换到 actuator order 并应用
        self.data.ctrl[:] = tau_joint[ACTUATOR_TO_JOINT]

    def _compute_gravity_compensation(self):
        """计算重力补偿力矩"""
        if self.fsm_state != 'RL_MIX':
            return

        qacc_save = self.data.qacc.copy()
        qfrc_save = self.data.qfrc_inverse.copy()

        self.data.qacc[:] = 0.0
        mujoco.mj_inverse(self.model, self.data)

        # qfrc_inverse 是 dof order，取关节部分 (dofadr 6:18)
        # 但这是 dof order，需要映射到 joint order
        # 对于 hinge joints, dof order = joint order (每个 hinge 1 DOF)
        self._gravity_comp = self.data.qfrc_inverse[6:18].copy()

        self.data.qacc[:] = qacc_save
        self.data.qfrc_inverse[:] = qfrc_save

    def _update_velocity_cmd(self):
        vx, vy, wz = 0.0, 0.0, 0.0
        if self.key_w: vx += 0.5
        if self.key_s: vx -= 0.5
        if self.key_a: vy += 0.5
        if self.key_d: vy -= 0.5
        if self.key_q: wz += 0.5
        if self.key_e: wz -= 0.5
        VELOCITY_CMD[:] = [vx, vy, wz]

    def _has_movement_key(self):
        return any([self.key_w, self.key_s, self.key_a, self.key_d, self.key_q, self.key_e])

    def _update_fsm(self):
        if self.fsm_state == 'PASSIVE' and self.key_standup:
            self.fsm_state = 'STANDUP'
            self.standup_start_time = self.data.time
            self.target_pos = STAND_JOINT_POS.copy()
            self.key_standup = False
            print(f"\n[FSM] PASSIVE -> STANDUP (t={self.data.time:.1f}s)")

        elif self.fsm_state == 'STANDUP':
            if self.data.time - self.standup_start_time >= self.standup_duration:
                self.fsm_state = 'BALANCE'
                print(f"\n[FSM] STANDUP -> BALANCE (t={self.data.time:.1f}s)")

        elif self.fsm_state == 'BALANCE':
            if self._has_movement_key():
                self._update_velocity_cmd()
                self.fsm_state = 'RL_MIX'
                print(f"\n[FSM] BALANCE -> RL_MIX (t={self.data.time:.1f}s)")
                print(f"      速度命令: vx={VELOCITY_CMD[0]:.1f}, vy={VELOCITY_CMD[1]:.1f}, wz={VELOCITY_CMD[2]:.1f}")

        elif self.fsm_state == 'RL_MIX':
            if not self._has_movement_key():
                self.fsm_state = 'BALANCE'
                VELOCITY_CMD[:] = [0.0, 0.0, 0.0]
                print(f"\n[FSM] RL_MIX -> BALANCE (t={self.data.time:.1f}s)")
            elif self.key_liedown:
                self.fsm_state = 'STANDUP'
                self.standup_start_time = self.data.time
                self.target_pos = LIE_JOINT_POS.copy()
                self.key_liedown = False
                VELOCITY_CMD[:] = [0.0, 0.0, 0.0]
            else:
                self._update_velocity_cmd()

    def _interpolate_target(self):
        if self.fsm_state != 'STANDUP':
            return
        elapsed = self.data.time - self.standup_start_time
        progress = min(elapsed / self.standup_duration, 1.0)
        t = progress * progress * (3 - 2 * progress)
        self.target_pos = LIE_JOINT_POS + t * (STAND_JOINT_POS - LIE_JOINT_POS)
        self._standup_gain = min(elapsed / STANDUP_GAIN_RAMP_DURATION, 1.0)

    def run_step(self):
        self._update_fsm()

        if self.fsm_state == 'STANDUP':
            self._interpolate_target()
            return None, None

        elif self.fsm_state == 'BALANCE':
            self.target_pos = STAND_JOINT_POS.copy()
            return None, None

        elif self.fsm_state == 'RL_MIX':
            obs = self._get_observation()
            input_name = self.session.get_inputs()[0].name
            output_name = self.session.get_outputs()[0].name
            actions = self.session.run([output_name], {input_name: obs.reshape(1, -1)})[0][0]

            print(f"[DEBUG] ONNX: [{actions.min():.3f}, {actions.max():.3f}]  "
                  f"lin_vel={obs[0:3].round(3)} grav={obs[6:9].round(3)} "
                  f"jpos_rel={obs[24:27].round(3)}")

            # Isaac Lab action scale = 0.25
            actions_mj = actions[ISAAC_TO_MUJOCO]
            self.target_pos = STAND_JOINT_POS + actions_mj * 0.25
            self.last_actions = actions.copy()

            return obs, actions

        return None, None

    def run(self):
        print("\n[INFO] 启动 MuJoCo 仿真 (Go2)")
        print("      FSM 状态: PASSIVE (趴着)")
        print("\n[控制说明]")
        print("      U 键:     站立")
        print("      Space 键: 趴下")
        print("      W/S:      前进/后退")
        print("      A/D:      左移/右移")
        print("      Q/E:      左转/右转")
        print("      松键:     保持站立\n")

        base_pos = self.data.qpos[0:3]
        print(f"[初始状态]")
        print(f"  基座位置: ({base_pos[0]:.3f}, {base_pos[1]:.3f}, {base_pos[2]:.3f})")
        print(f"  关节位置: {self.data.qpos[7:19]}")
        print(f"  站立姿态: {STAND_JOINT_POS}")

        with mujoco.viewer.launch_passive(self.model, self.data) as viewer:
            viewer._render_ui = False

            if HAS_PYNPUT:
                def on_press(key):
                    try:
                        c = key.char
                        if c == 'u': self.key_standup = True
                        elif c == ' ': self.key_liedown = True
                        elif c == 'w': self.key_w = True
                        elif c == 's': self.key_s = True
                        elif c == 'a': self.key_a = True
                        elif c == 'd': self.key_d = True
                        elif c == 'q': self.key_q = True
                        elif c == 'e': self.key_e = True
                    except AttributeError:
                        if key == pynput_keyboard.Key.space:
                            self.key_liedown = True

                def on_release(key):
                    try:
                        c = key.char
                        if c == 'w': self.key_w = False
                        elif c == 's': self.key_s = False
                        elif c == 'a': self.key_a = False
                        elif c == 'd': self.key_d = False
                        elif c == 'q': self.key_q = False
                        elif c == 'e': self.key_e = False
                    except AttributeError:
                        pass

                listener = pynput_keyboard.Listener(on_press=on_press, on_release=on_release)
                listener.start()
                print("[INFO] 全局键盘监听已启动 (pynput)")
            else:
                listener = None

            step = 0
            while viewer.is_running():
                start_time = time.time()
                self._update_fsm()

                # 策略更新
                if self.fsm_state == 'STANDUP':
                    self._interpolate_target()
                elif self.fsm_state == 'BALANCE':
                    self.target_pos = STAND_JOINT_POS.copy()
                elif self.fsm_state == 'RL_MIX':
                    obs = self._get_observation()
                    input_name = self.session.get_inputs()[0].name
                    output_name = self.session.get_outputs()[0].name
                    actions = self.session.run([output_name], {input_name: obs.reshape(1, -1)})[0][0]

                    print(f"[DEBUG] ONNX (raw): [{actions.min():.3f}, {actions.max():.3f}]  "
                          f"lin_vel={obs[0:3].round(3)} grav={obs[6:9].round(3)} "
                          f"jpos_rel={obs[24:27].round(3)}")

                    # 裁剪 ONNX 输出到 [-1, 1] (匹配训练时的动作范围)
                    actions = np.clip(actions, -1.0, 1.0)
                    print(f"[DEBUG] ONNX (clipped): [{actions.min():.3f}, {actions.max():.3f}]")

                    actions_mj = actions[ISAAC_TO_MUJOCO]
                    self.target_pos = STAND_JOINT_POS + actions_mj * 0.25
                    self.last_actions = actions.copy()

                # 物理子步循环
                for _ in range(self.steps_per_control):
                    self._compute_gravity_compensation()
                    if self.fsm_state == 'STANDUP':
                        kp = KP_STANDUP * self._standup_gain
                        kd = KD_STANDUP * self._standup_gain
                        self._apply_pd_control(kp, kd)
                    elif self.fsm_state == 'BALANCE':
                        self._apply_pd_control(KP_BALANCE, KD_BALANCE)
                    else:
                        self._apply_pd_control()
                    mujoco.mj_step(self.model, self.data)

                viewer.sync()

                step += 1
                if step % 10 == 0:
                    bp = self.data.qpos[0:3]
                    qt = self.data.qpos[3:7]
                    print(f"[Step {step}] FSM={self.fsm_state} "
                          f"pos=({bp[0]:.2f}, {bp[1]:.2f}, {bp[2]:.2f}) "
                          f"quat=({qt[0]:.3f},{qt[1]:.3f},{qt[2]:.3f},{qt[3]:.3f})")

                elapsed = time.time() - start_time
                if elapsed < CONTROL_DT:
                    time.sleep(CONTROL_DT - elapsed)

        print("\n[INFO] 仿真结束")
        if HAS_PYNPUT and listener:
            listener.stop()


def main():
    onnx_path = sys.argv[1] if len(sys.argv) > 1 else DEFAULT_ONNX_PATH
    if not os.path.exists(onnx_path):
        print(f"[ERROR] ONNX 模型不存在: {onnx_path}")
        sys.exit(1)
    runner = Go2PolicyRunner(onnx_path)
    runner.run()


if __name__ == "__main__":
    main()
