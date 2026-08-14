#!/usr/bin/env python3
"""
XGB 四足机器人 ONNX 策略推理 + MuJoCo 仿真

使用 Isaac Lab 训练导出的 policy.onnx 控制 xgb 机器人。

用法:
    python run_onnx_policy.py POLICY.onnx [--mjcf scene.xml]
"""

import argparse
import numpy as np
import mujoco
import mujoco.viewer
import onnxruntime as ort
import time
import os

try:
    from pynput import keyboard as pynput_keyboard
    HAS_PYNPUT = True
except ImportError:
    HAS_PYNPUT = False

# ==================== 配置 ====================

# PD 控制参数 (匹配 Isaac Lab xgb.py)
KP_RL = 20.0
KD_RL = 0.7

# 默认关节位置 (对齐 Matrix)
# 站立姿态: ABAD=0, HIP=0.8, KNEE=-1.5
STAND_JOINT_POS = np.array([
    0.0,  0.8, -1.5,  # FAR: ABAD, HIP, KNEE
    0.0,  0.8, -1.5,  # FBL
    0.0,  0.8, -1.5,  # RAR
    0.0,  0.8, -1.5,  # RBL
])

# 速度命令 (由按键动态设置)
VELOCITY_CMD = np.array([0.0, 0.0, 0.0])  # vx, vy, wz (初始为0)

# 控制频率 (匹配 Isaac Lab: sim.dt=0.005 × decimation=4 = 0.02s = 50Hz)
CONTROL_DT = 0.02  # 50Hz (策略更新频率)
ACTION_SCALE = 0.25

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

MUJOCO_JOINT_NAMES = [
    f"{leg}_{joint}_JOINT"
    for leg in ("FAR", "FBL", "RAR", "RBL")
    for joint in ("ABAD", "HIP", "KNEE")
]


class XgbPolicyRunner:
    """XGB 机器人 ONNX 策略推理器"""

    def __init__(self, onnx_path: str, mjcf_path: str):
        # 加载 ONNX 模型
        print(f"[INFO] 加载 ONNX 模型: {onnx_path}")
        # 使用 CPU (CUDA 可能不兼容当前环境)
        self.session = ort.InferenceSession(onnx_path, providers=['CPUExecutionProvider'])
        print(f"[OK] ONNX 模型加载成功")
        print(f"     输入: {self.session.get_inputs()[0].name} {self.session.get_inputs()[0].shape}")
        print(f"     输出: {self.session.get_outputs()[0].name} {self.session.get_outputs()[0].shape}")

        # 加载 MuJoCo 模型
        print(f"\n[INFO] 加载 MuJoCo 模型: {mjcf_path}")
        self.model = mujoco.MjModel.from_xml_path(mjcf_path)
        self.data = mujoco.MjData(self.model)
        print(f"[OK] MuJoCo 模型加载成功")
        print(f"     关节数: {self.model.njnt}")
        print(f"     执行器数: {self.model.nu}")
        print(f"     物理步长: {self.model.opt.timestep:.4f}s ({1/self.model.opt.timestep:.0f}Hz)")

        self._validate_interfaces()

        # 根据实际模型步长计算每控制步的物理子步数
        step_ratio = CONTROL_DT / self.model.opt.timestep
        self.steps_per_control = round(step_ratio)
        if self.steps_per_control < 1 or not np.isclose(step_ratio, self.steps_per_control):
            raise ValueError(
                f"策略周期 {CONTROL_DT}s 不是 MuJoCo 步长 "
                f"{self.model.opt.timestep}s 的整数倍"
            )
        print(f"     策略频率: {1/CONTROL_DT:.0f}Hz (每 {self.steps_per_control} 个物理步)")

        # 初始化状态
        self.last_actions = np.zeros(12)
        self.control_time = 0.0

        # 按键状态
        # 运动控制按键
        self.key_w = False  # 前进
        self.key_s = False  # 后退
        self.key_a = False  # 左移
        self.key_d = False  # 右移
        self.key_q = False  # 左转
        self.key_e = False  # 右转

        # PD 目标位置
        self.target_pos = STAND_JOINT_POS.copy()

        # 直接从训练环境的默认站立状态运行策略，避免部署侧 FSM 增益切换。
        self._reset_joints()

    def _validate_interfaces(self):
        """在启动时验证 ONNX 形状以及 MuJoCo 关节/执行器顺序。"""
        input_shape = self.session.get_inputs()[0].shape
        output_shape = self.session.get_outputs()[0].shape
        if input_shape[-1] != 48 or output_shape[-1] != 12:
            raise ValueError(
                f"期望 ONNX 输入/输出为 48/12 维，实际为 "
                f"{input_shape}/{output_shape}"
            )

        joint_ids = np.array([
            mujoco.mj_name2id(self.model, mujoco.mjtObj.mjOBJ_JOINT, name)
            for name in MUJOCO_JOINT_NAMES
        ])
        if np.any(joint_ids < 0):
            missing = [
                name for name, joint_id in zip(MUJOCO_JOINT_NAMES, joint_ids)
                if joint_id < 0
            ]
            raise ValueError(f"MuJoCo 模型缺少关节: {missing}")

        qpos_adr = self.model.jnt_qposadr[joint_ids]
        dof_adr = self.model.jnt_dofadr[joint_ids]
        actuator_joint_ids = self.model.actuator_trnid[:, 0]
        if not np.array_equal(qpos_adr, np.arange(7, 19)):
            raise ValueError(f"关节 qpos 顺序不匹配: {qpos_adr}")
        if not np.array_equal(dof_adr, np.arange(6, 18)):
            raise ValueError(f"关节 qvel 顺序不匹配: {dof_adr}")
        if not np.array_equal(actuator_joint_ids, joint_ids):
            raise ValueError(
                "actuator 顺序与 FAR/FBL/RAR/RBL 的 ABAD/HIP/KNEE 顺序不匹配"
            )

    def _reset_joints(self):
        """重置到 Isaac Lab 训练的默认站立状态。"""
        # free joint: qpos[0:3]=pos(x,y,z), qpos[3:7]=quat(w,x,y,z)
        # hinge joints: qpos[7:19]
        self.data.qpos[7:19] = STAND_JOINT_POS
        self.data.qpos[0:3] = [0.0, 0.0, 0.32]
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
        # 基座四元数 (w, x, y, z) - 从 qpos 直接读取
        quat = self.data.qpos[3:7].copy()  # w, x, y, z

        # 基座线速度 (本体坐标系)
        # qvel[0:3] 是世界坐标系，需要转换
        base_lin_vel_world = self.data.qvel[0:3].copy()
        base_lin_vel = self._world_to_body(base_lin_vel_world, quat)

        # MuJoCo free joint 的角速度 qvel[3:6] 已在本体坐标系。
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

    def _infer_policy(self):
        """以 Isaac Lab 的观测和关节顺序运行一次策略。"""
        obs = self._get_observation()
        input_name = self.session.get_inputs()[0].name
        output_name = self.session.get_outputs()[0].name
        actions = self.session.run(
            [output_name], {input_name: obs.reshape(1, -1)}
        )[0][0]
        if not np.all(np.isfinite(actions)):
            raise RuntimeError(f"ONNX 输出包含非有限值: {actions}")

        # 按当前部署假设做 scale + default offset；需用 rough_env_cfg.py 最终核对。
        actions_mj = actions[ISAAC_TO_MUJOCO]
        self.target_pos = STAND_JOINT_POS + actions_mj * ACTION_SCALE
        self.last_actions = actions.copy()
        return obs, actions

    def run(self):
        """从默认站立状态直接运行 ONNX 策略。"""
        print("\n[INFO] 启动 MuJoCo 仿真")
        print("      ONNX 策略已启用，初始速度命令为零")
        print("\n[控制说明]")
        print("      W 键:     前进 (vx=+0.5)")
        print("      S 键:     后退 (vx=-0.5)")
        print("      A 键:     左移 (vy=+0.5)")
        print("      D 键:     右移 (vy=-0.5)")
        print("      Q 键:     左转 (wz=+0.5)")
        print("      E 键:     右转 (wz=-0.5)")
        print("      松键:     零速度命令（ONNX 仍持续运行）")
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
        print(f"  站立姿态: {STAND_JOINT_POS}")

        with mujoco.viewer.launch_passive(self.model, self.data) as viewer:
            # 禁用 MuJoCo UI（防止快捷键冲突）
            viewer._render_ui = False

            # 全局键盘监听（不依赖窗口焦点）
            if HAS_PYNPUT:
                def on_press(key):
                    try:
                        c = key.char
                        if c == 'w': self.key_w = True
                        elif c == 's': self.key_s = True
                        elif c == 'a': self.key_a = True
                        elif c == 'd': self.key_d = True
                        elif c == 'q': self.key_q = True
                        elif c == 'e': self.key_e = True
                    except AttributeError:
                        pass

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
                print("[WARN] pynput 未安装，无法使用键盘控制")
                print("       安装: pip install pynput")

            step = 0
            while viewer.is_running():
                start_time = time.time()

                # 零命令时也持续跑 ONNX，按键只修改 command observation。
                self._update_velocity_cmd()
                obs, actions = self._infer_policy()

                if step % 10 == 0:
                    print(f"[DEBUG] ONNX: [{actions.min():.3f}, {actions.max():.3f}]  "
                          f"lin_vel={obs[0:3].round(3)} "
                          f"ang_vel={obs[3:6].round(3)} "
                          f"grav={obs[6:9].round(3)} "
                          f"jpos_rel={obs[12:15].round(3)} "
                          f"jvel={obs[24:27].round(3)}")

                # 物理子步循环
                for _ in range(self.steps_per_control):
                    self._apply_pd_control()
                    mujoco.mj_step(self.model, self.data)

                # 更新可视化
                viewer.sync()

                # 打印状态
                step += 1
                if step % 10 == 0:
                    base_pos = self.data.qpos[0:3]
                    quat = self.data.qpos[3:7]
                    print(f"[Step {step}] cmd={VELOCITY_CMD.round(2)} "
                          f"pos=({base_pos[0]:.2f}, {base_pos[1]:.2f}, {base_pos[2]:.2f}) "
                          f"quat=({quat[0]:.3f},{quat[1]:.3f},{quat[2]:.3f},{quat[3]:.3f})")

                # 控制循环频率
                elapsed = time.time() - start_time
                if elapsed < CONTROL_DT:
                    time.sleep(CONTROL_DT - elapsed)

        print("\n[INFO] 仿真结束")

        # 清理键盘监听
        if HAS_PYNPUT and listener is not None:
            listener.stop()


def main():
    parser = argparse.ArgumentParser(description="XGB ONNX policy MuJoCo runner")
    parser.add_argument("policy", help="Isaac Lab 导出的 XGB policy.onnx 路径")
    parser.add_argument(
        "--mjcf",
        default=os.environ.get("XGB_MJCF_PATH"),
        required="XGB_MJCF_PATH" not in os.environ,
        help="原工程 XGB scene.xml 路径（也可设置 XGB_MJCF_PATH）",
    )
    parser.add_argument(
        "--check-only",
        action="store_true",
        help="只检查 ONNX/MJCF/映射并运行一次推理，不启动可视化",
    )
    args = parser.parse_args()

    onnx_path = os.path.abspath(os.path.expanduser(args.policy))
    mjcf_path = os.path.abspath(os.path.expanduser(args.mjcf))
    if not os.path.isfile(onnx_path):
        parser.error(f"ONNX 模型不存在: {onnx_path}")
    if not os.path.isfile(mjcf_path):
        parser.error(f"MJCF 模型不存在: {mjcf_path}")

    # 运行策略
    runner = XgbPolicyRunner(onnx_path, mjcf_path)
    if args.check_only:
        obs, actions = runner._infer_policy()
        print("[OK] 接口自检通过")
        print(f"     observation: shape={obs.shape}, finite={np.all(np.isfinite(obs))}")
        print(
            f"     action: shape={actions.shape}, finite={np.all(np.isfinite(actions))}, "
            f"range=[{actions.min():.4f}, {actions.max():.4f}]"
        )
        return
    runner.run()


if __name__ == "__main__":
    main()
