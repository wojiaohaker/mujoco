#!/usr/bin/env python3
"""
XGB 四足机器人 ONNX 策略推理 + MuJoCo 仿真

使用 Isaac Lab 训练导出的 policy.onnx 控制 xgb 机器人。

用法:
    python run_onnx_policy.py [onnx_path]

默认 ONNX 路径:
    ~/Softwares/IsaacLab/logs/rsl_rl/xgb_flat/2026-08-17_15-45-34/exported/policy.onnx
"""

import numpy as np
import mujoco
import mujoco.viewer
import onnxruntime as ort
import time
import sys
import os
import threading

# ==================== 配置 ====================

# ONNX 模型路径
DEFAULT_ONNX_PATH = os.path.expanduser(
    "~/Softwares/xgbrl/logs/rsl_rl/xgb_flat/2026-08-17_15-45-34/exported/policy.onnx"
)

# MuJoCo 模型路径
MJCF_PATH = os.path.expanduser(
    "~/Softwares/Matrix/src/robot_mujoco/zsibot_robots/xgb/scene.xml"
)

# PD 控制参数 (精确匹配 Isaac Lab xgb.py: stiffness=20, damping=0.7)
KP_RL = 20.0       # RL 模式 Kp (匹配 Isaac Lab XGB_ACTUATOR_CFG.stiffness)
KD_RL = 0.7        # RL 模式 Kd (匹配 Isaac Lab XGB_ACTUATOR_CFG.damping)
KP_STANDUP = 150.0  # 站立模式 Kp (用户要求: 150)
KD_STANDUP = 2.0    # 站立模式 Kd (用户要求: 2.0)
STANDUP_GAIN_RAMP_DURATION = 1.0  # 增益渐进时间 (1.0s)
ACTION_SCALE = 0.25  # Isaac Lab action scale

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

# 速度命令 (由按键动态设置)
VELOCITY_CMD = np.array([0.0, 0.0, 0.0])  # vx, vy, wz (初始为0)

# 控制频率 (匹配 Isaac Lab: sim.dt=0.005 × decimation=4 = 0.02s = 50Hz)
CONTROL_DT = 0.02  # 50Hz (策略更新频率)

# ==================== 关节顺序映射 ====================

# Isaac Lab Articulation 关节顺序 (按类型分组，从 play 终端输出确认):
# [FAR_ABAD, FBL_ABAD, RAR_ABAD, RBL_ABAD,   ← ABAD×4
#  FAR_HIP,  FBL_HIP,  RAR_HIP,  RBL_HIP,    ← HIP×4
#  FAR_KNEE, FBL_KNEE, RAR_KNEE, RBL_KNEE]   ← KNEE×4
#
# MuJoCo qpos[7:19] / actuator 顺序 (深度优先，按腿分组):
# [FAR_ABAD, FAR_HIP, FAR_KNEE,               ← 前右腿
#  FBL_ABAD, FBL_HIP, FBL_KNEE,               ← 前左腿
#  RAR_ABAD, RAR_HIP, RAR_KNEE,               ← 后右腿
#  RBL_ABAD, RBL_HIP, RBL_KNEE]               ← 后左腿

# MuJoCo → Isaac Lab: 从 leg-grouped 重排到 type-grouped
MUJOCO_TO_ISAAC = np.array([0, 3, 6, 9, 1, 4, 7, 10, 2, 5, 8, 11])
# Isaac Lab → MuJoCo: 从 type-grouped 重排到 leg-grouped
ISAAC_TO_MUJOCO = np.array([0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7, 11])


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

        # 根据实际模型步长计算每控制步的物理子步数
        self.steps_per_control = max(1, round(CONTROL_DT / self.model.opt.timestep))
        print(f"     策略频率: {1/CONTROL_DT:.0f}Hz (每 {self.steps_per_control} 个物理步)")

        # 初始化状态
        self.last_actions = np.zeros(12)
        self.control_time = 0.0

        # LSTM 隐藏状态 (512维，匹配训练配置)
        self.lstm_h = np.zeros((1, 1, 512), dtype=np.float32)  # h_in
        self.lstm_c = np.zeros((1, 1, 512), dtype=np.float32)  # c_in
        self._has_lstm = len(self.session.get_inputs()) > 1  # 检测是否为 LSTM 模型

        # FSM 状态: 'PASSIVE' -> 'STANDUP' -> 'BALANCE' <-> 'RL_MIX'
        self.fsm_state = 'PASSIVE'
        self.standup_start_time = None
        self.standup_duration = 3.0  # 站立过渡总时间 (Phase A=1s + Phase B=2s)
        self._standup_gain = 0.0     # 站立增益渐进系数

        # 按键状态
        self.key_standup = False   # U 键：站立
        self.key_liedown = False   # Space 键：趴下
        # 运动控制按键
        self.key_w = False  # 前进
        self.key_s = False  # 后退
        self.key_a = False  # 左移
        self.key_d = False  # 右移
        self.key_q = False  # 左转
        self.key_e = False  # 右转

        # PD 目标位置
        self.target_pos = LIE_JOINT_POS.copy()  # 初始目标：趴下姿态

        # 设置初始关节位置 (趴下姿态，身体贴近地面)
        self._reset_joints()

    def _reset_joints(self):
        """重置关节到趴下位置，身体贴近地面"""
        # free joint: qpos[0:3]=pos(x,y,z), qpos[3:7]=quat(w,x,y,z)
        # hinge joints: qpos[7:19]
        self.data.qpos[7:19] = LIE_JOINT_POS
        self.data.qpos[0:3] = [0.0, 0.0, 0.12]   # 基座位置 (贴近地面)
        self.data.qpos[3:7] = [1.0, 0.0, 0.0, 0.0]  # 四元数 (无旋转)
        mujoco.mj_forward(self.model, self.data)

    def _get_joint_pos_mujoco(self) -> np.ndarray:
        """获取 MuJoCo 关节位置 (12维)"""
        return self.data.qpos[7:19].copy()

    def _get_joint_vel_mujoco(self) -> np.ndarray:
        """获取 MuJoCo 关节速度 (12维)"""
        return self.data.qvel[6:18].copy()

    def _get_observation(self) -> np.ndarray:
        """
        构造 48 维观测 (Matrix 格式)

        观测结构 (与 Matrix robot_mc 一致):
        - projected_gravity: 3 (重力投影, 本体坐标系)
        - base_ang_vel: 3    (基座角速度, 本体坐标系)
        - base_lin_vel: 3    (基座线速度, 本体坐标系; 部署时用 odom 估计)
        - velocity_commands: 3 (速度命令)
        - joint_pos: 12      (关节相对位置, Isaac Lab 顺序)
        - joint_vel: 12      (关节速度, Isaac Lab 顺序)
        - last_actions: 12   (上一步动作, Isaac Lab 顺序)
        """
        # 基座四元数 (w, x, y, z) - 从 qpos 直接读取
        quat = self.data.qpos[3:7].copy()  # w, x, y, z

        # 基座线速度 (本体坐标系)
        # qvel[0:3] 是世界坐标系，需要转换
        base_lin_vel_world = self.data.qvel[0:3].copy()
        base_lin_vel = self._world_to_body(base_lin_vel_world, quat)

        # 基座角速度 (本体坐标系)
        # MuJoCo free joint 的 qvel[3:6] 已在本体坐标系，无需转换。
        base_ang_vel = self.data.qvel[3:6].copy()

        # 重力投影 (本体坐标系)
        gravity_world = np.array([0, 0, -1])
        projected_gravity = self._world_to_body(gravity_world, quat)

        # 关节位置和速度 (MuJoCo 顺序 -> Isaac Lab 顺序)
        # Isaac Lab 使用 joint_pos_rel (相对默认姿态的偏差)
        joint_pos_mj = self._get_joint_pos_mujoco()
        joint_vel_mj = self._get_joint_vel_mujoco()

        # 先在 MuJoCo 顺序下计算相对偏差，再转换到 Isaac Lab 顺序
        # STAND_JOINT_POS 是 MuJoCo 顺序（按腿分组：FAR→FBL→RAR→RBL）
        joint_pos_rel_mj = joint_pos_mj - STAND_JOINT_POS
        joint_pos_rel = joint_pos_rel_mj[MUJOCO_TO_ISAAC]  # 转换到 Isaac Lab 顺序

        joint_vel_isaac = joint_vel_mj[MUJOCO_TO_ISAAC]
        # joint_vel_rel = joint_vel - 0 = joint_vel (默认速度为0)

        # 构造观测 (Matrix 顺序: grav → ang_vel → lin_vel → cmd → pos → vel → action)
        obs = np.concatenate([
            projected_gravity,  # 3
            base_ang_vel,       # 3
            base_lin_vel,       # 3
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
        τ = Kp * (target - q) - Kd * q̇
        """
        if kp is None:
            kp = KP_RL
        if kd is None:
            kd = KD_RL

        current_pos = self.data.qpos[7:19]
        current_vel = self.data.qvel[6:18]

        # Isaac Lab 训练侧的 implicit PD 没有额外的重力补偿项。
        tau = kp * (self.target_pos - current_pos) - kd * current_vel

        # 限幅 ±28 Nm
        tau = np.clip(tau, -28.0, 28.0)

        # 应用到执行器
        self.data.ctrl[:] = tau

    def _update_velocity_cmd(self):
        """根据按键更新速度命令"""
        vx, vy, wz = 0.0, 0.0, 0.0
        if self.key_w: vx += 0.5
        if self.key_s: vx -= 0.5
        if self.key_a: vy += 0.5
        if self.key_d: vy -= 0.5
        if self.key_q: wz += 0.5
        if self.key_e: wz -= 0.5
        VELOCITY_CMD[:] = [vx, vy, wz]

    def _has_movement_key(self):
        """检查是否有运动按键按下"""
        return self.key_w or self.key_s or self.key_a or self.key_d or self.key_q or self.key_e

    def _update_fsm(self):
        """FSM 状态转换"""
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
                print(f"      保持站立，按 W/S/A/D/Q/E 驱动 ONNX 策略")

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
                print(f"\n[FSM] RL_MIX -> STANDUP (趴下过渡) (t={self.data.time:.1f}s)")
            else:
                self._update_velocity_cmd()

    def _interpolate_target(self):
        """站立过渡：平滑插值关节目标 + 增益渐进"""
        elapsed = self.data.time - self.standup_start_time
        progress = min(elapsed / self.standup_duration, 1.0)
        # 平滑插值 (ease-in-out)
        t = progress * progress * (3 - 2 * progress)
        # 从趴下姿态插值到站立姿态
        self.target_pos = LIE_JOINT_POS + t * (STAND_JOINT_POS - LIE_JOINT_POS)
        # 增益渐进：前 1 秒从 0 线性增长到 1.0
        gain_progress = min(elapsed / STANDUP_GAIN_RAMP_DURATION, 1.0)
        self._standup_gain = gain_progress

    def _infer_policy(self):
        """以 Matrix 格式的观测和关节顺序运行一次策略（支持 LSTM）。"""
        obs = self._get_observation()

        if self._has_lstm:
            # LSTM 模型: obs + h_in + c_in → actions + h_out + c_out
            input_names = [inp.name for inp in self.session.get_inputs()]
            output_names = [out.name for out in self.session.get_outputs()]
            feed = {
                input_names[0]: obs.reshape(1, -1),   # obs [1, 48]
                input_names[1]: self.lstm_h,           # h_in [1, 1, 512]
                input_names[2]: self.lstm_c,           # c_in [1, 1, 512]
            }
            results = self.session.run(output_names, feed)
            actions = results[0][0]                    # actions [12]
            self.lstm_h = results[1]                   # h_out → 下一步的 h_in
            self.lstm_c = results[2]                   # c_out → 下一步的 c_in
        else:
            # 简单 MLP 模型 (无 LSTM)
            input_name = self.session.get_inputs()[0].name
            output_name = self.session.get_outputs()[0].name
            actions = self.session.run(
                [output_name], {input_name: obs.reshape(1, -1)}
            )[0][0]

        if not np.all(np.isfinite(actions)):
            raise RuntimeError(f"ONNX 输出包含非有限值: {actions}")

        # 不裁剪 ONNX 输出 (Isaac Lab 训练时不裁剪)
        actions_mj = actions[ISAAC_TO_MUJOCO]
        self.target_pos = STAND_JOINT_POS + actions_mj * ACTION_SCALE
        self.last_actions = actions.copy()
        return obs, actions

    def run(self):
        """运行仿真循环 (FSM: PASSIVE -> STANDUP -> BALANCE <-> RL_MIX)"""
        print("\n[INFO] 启动 MuJoCo 仿真")
        print("      FSM 状态: PASSIVE (趴着)")
        print("\n[控制说明]")
        print("      U 键:     站立 (PASSIVE -> STANDUP -> BALANCE)")
        print("      Space 键: 趴下")
        print("      W 键:     前进 (vx=+0.5)")
        print("      S 键:     后退 (vx=-0.5)")
        print("      A 键:     左移 (vy=+0.5)")
        print("      D 键:     右移 (vy=-0.5)")
        print("      Q 键:     左转 (wz=+0.5)")
        print("      E 键:     右转 (wz=-0.5)")
        print("      松键:     保持站立 (BALANCE)")
        print("      鼠标左键拖拽: 旋转视角")
        print("      鼠标右键拖拽: 平移视角")
        print("      滚轮:         缩放")
        print("      关闭窗口:     退出\n")

        # 打印初始状态
        base_pos = self.data.qpos[0:3]
        joint_pos = self.data.qpos[7:19]
        print(f"[初始状态]")
        print(f"  基座位置: ({base_pos[0]:.3f}, {base_pos[1]:.3f}, {base_pos[2]:.3f})")
        print(f"  基座四元数: {self.data.qpos[3:7]}")
        print(f"  关节位置: {joint_pos}")
        print(f"  趴下姿态: {LIE_JOINT_POS}")
        print(f"  站立姿态: {STAND_JOINT_POS}")

        with mujoco.viewer.launch_passive(
            self.model, self.data,
            show_left_ui=False,
            show_right_ui=False,
        ) as viewer:

            # X11 全局按键拦截（防止与 MuJoCo 快捷键冲突）
            # MuJoCo 的 'W'=Wireframe, 'S'=Shadow, 'E'=Equality 等
            # 使用 XGrabKey 在根窗口拦截，按键不会到达 MuJoCo
            from Xlib import display as x11_display, X as x11_X, XK as x11_XK
            import queue

            _key_queue = queue.Queue()
            _x11_dpy = None

            def _x11_key_grabber():
                """X11 后台线程：拦截控制键，防止到达 MuJoCo"""
                nonlocal _x11_dpy
                _x11_dpy = x11_display.Display()
                root = _x11_dpy.screen().root

                keys_to_grab = {
                    'w': x11_XK.string_to_keysym('w'),
                    's': x11_XK.string_to_keysym('s'),
                    'a': x11_XK.string_to_keysym('a'),
                    'd': x11_XK.string_to_keysym('d'),
                    'q': x11_XK.string_to_keysym('q'),
                    'e': x11_XK.string_to_keysym('e'),
                    'u': x11_XK.string_to_keysym('u'),
                    'space': x11_XK.string_to_keysym('space'),
                }

                for name, keysym in keys_to_grab.items():
                    keycode = _x11_dpy.keysym_to_keycode(keysym)
                    # owner_events=False: 按键事件只发给 grabber，不发给 MuJoCo
                    root.grab_key(keycode, x11_X.AnyModifier, False,
                                  x11_X.GrabModeAsync, x11_X.GrabModeAsync)

                _x11_dpy.flush()
                print("[INFO] X11 控制键拦截已启用 (W/S/A/D/Q/E/U/Space)")

                while True:
                    event = _x11_dpy.next_event()
                    if event.type == x11_X.KeyPress:
                        for name, keysym in keys_to_grab.items():
                            keycode = _x11_dpy.keysym_to_keycode(keysym)
                            if event.detail == keycode:
                                _key_queue.put(('press', name))
                                break
                    elif event.type == x11_X.KeyRelease:
                        for name, keysym in keys_to_grab.items():
                            keycode = _x11_dpy.keysym_to_keycode(keysym)
                            if event.detail == keycode:
                                _key_queue.put(('release', name))
                                break

            _grabber_thread = threading.Thread(target=_x11_key_grabber, daemon=True)
            _grabber_thread.start()
            time.sleep(0.3)  # 等待 X11 grab 生效

            # 处理按键事件的辅助函数
            def _process_key_events():
                while not _key_queue.empty():
                    try:
                        event_type, key_name = _key_queue.get_nowait()
                        if event_type == 'press':
                            if key_name == 'u': self.key_standup = True
                            elif key_name == 'w': self.key_w = True
                            elif key_name == 's': self.key_s = True
                            elif key_name == 'a': self.key_a = True
                            elif key_name == 'd': self.key_d = True
                            elif key_name == 'q': self.key_q = True
                            elif key_name == 'e': self.key_e = True
                            elif key_name == 'space': self.key_liedown = True
                        elif event_type == 'release':
                            if key_name == 'w': self.key_w = False
                            elif key_name == 's': self.key_s = False
                            elif key_name == 'a': self.key_a = False
                            elif key_name == 'd': self.key_d = False
                            elif key_name == 'q': self.key_q = False
                            elif key_name == 'e': self.key_e = False
                    except queue.Empty:
                        break

            step = 0
            while viewer.is_running():
                start_time = time.time()

                # 处理 X11 按键事件
                _process_key_events()

                # FSM 状态转换
                self._update_fsm()

                if self.fsm_state == 'STANDUP':
                    self._interpolate_target()
                elif self.fsm_state in ('BALANCE', 'RL_MIX'):
                    # BALANCE 和 RL_MIX 都跑 ONNX 策略
                    # BALANCE 时速度命令=0（策略学过的零速站立）
                    # RL_MIX 时速度命令由按键设置
                    obs, actions = self._infer_policy()
                    if step % 10 == 0:
                        print(f"[DEBUG] ONNX: [{actions.min():.3f}, {actions.max():.3f}]  "
                              f"cmd={VELOCITY_CMD.round(2)} "
                              f"grav={obs[0:3].round(3)} "
                              f"ang_vel={obs[3:6].round(3)} "
                              f"lin_vel={obs[6:9].round(3)}")

                # 物理子步循环
                for _ in range(self.steps_per_control):
                    if self.fsm_state == 'STANDUP':
                        kp = KP_STANDUP * self._standup_gain
                        kd = KD_STANDUP * self._standup_gain
                        self._apply_pd_control(kp, kd)
                    else:
                        # BALANCE 和 RL_MIX 统一使用 RL 增益 (KP=20, KD=0.7)
                        # ONNX 策略输出已经包含平衡所需的动作
                        self._apply_pd_control()
                    mujoco.mj_step(self.model, self.data)

                # 更新可视化
                viewer.sync()

                # 打印状态
                step += 1
                if step % 10 == 0:
                    base_pos = self.data.qpos[0:3]
                    quat = self.data.qpos[3:7]
                    print(f"[Step {step}] FSM={self.fsm_state} "
                          f"cmd={VELOCITY_CMD.round(2)} "
                          f"pos=({base_pos[0]:.2f}, {base_pos[1]:.2f}, {base_pos[2]:.2f}) "
                          f"quat=({quat[0]:.3f},{quat[1]:.3f},{quat[2]:.3f},{quat[3]:.3f})")

                # 控制循环频率
                elapsed = time.time() - start_time
                if elapsed < CONTROL_DT:
                    time.sleep(CONTROL_DT - elapsed)

        print("\n[INFO] 仿真结束")

        # 清理 X11 grab（关闭连接会自动释放所有 grab）
        if _x11_dpy is not None:
            try:
                _x11_dpy.close()
            except Exception:
                pass


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
