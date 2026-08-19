#!/usr/bin/env python3
"""
matrix_xgb: Matrix policy_mix_walk.onnx + MuJoCo 快速调试台

仿照 demo_xgb/run_onnx_policy.py 的结构，但换成 Matrix 原版策略
(qiyuan_mc/models/policy_mix_walk.onnx, md5 与 Matrix 加密目录一致)。

观测布局采用运行时定标结果 (LD_PRELOAD 钩子多次交叉验证):
  [0:2]   恒零 (NOT projected gravity)
  [3:5]   本体角速度 (qvel[3:6], MuJoCo free joint 已在本体系)
  [6:8]   估计器小量, 默认填零 (可配置)
  [9:11]  速度指令 vx*2.0, vy*2.0, wz*1.0
  [12:23] q - q_default (原始 MuJoCo 关节角, 腿序 FR,FL,RR,RL, 恒等映射)
  [24:35] qd
  [36:47] 上一步动作 (原始网络输出)

动作路径 (与 qiyuan_mc 一致, 可用 --no-clip 关闭裁剪做对比):
  q_target = q_default + 0.25 * clamp(action, -1, 1)
  PD: Kp=20, Kd=0.7

两种 RL 入口模式 (按键选择):
  R 键: 复刻 Matrix UE 模式 —— 从趴姿直接进 RL, 由策略自己站起
  U 键: PD 站立 (3s: Phase A 1s 收腿 + Phase B 2s 撑起, Kp=150/Kd=2.0)
        完成后直接进 RL

用法:
    python3 run_matrix_policy.py [onnx_path]
    python3 run_matrix_policy.py --no-clip      # 关闭动作裁剪
    python3 run_matrix_policy.py --d7 0.0401    # d6-8 填 (0,0.0401,0)
"""

import argparse
import os
import queue
import sys
import threading
import time

import mujoco
import mujoco.viewer
import numpy as np
import onnxruntime as ort

# ==================== 配置 ====================

DEFAULT_ONNX_PATH = os.path.expanduser(
    "~/Softwares/qiyuan_mc/models/policy_mix_walk.onnx")

MJCF_PATH = os.path.expanduser(
    "~/Softwares/Matrix/src/robot_mujoco/zsibot_robots/xgb/scene.xml")

# Matrix mc_ctrl RL 模式为 2ms 周期 (500Hz)
CONTROL_DT = 0.002

# RL PD 增益 (Matrix FSM_RL_*: Kp=20, Kd=0.7)
KP_RL = 20.0
KD_RL = 0.7

# PD 站立参数 (用户固定约束: 总时长 3s, Phase A=1s 收腿 + Phase B=2s 撑起)
KP_STANDUP = 150.0
KD_STANDUP = 2.0
STANDUP_GAIN_RAMP = 1.0
STANDUP_PHASE_A = 1.0
STANDUP_PHASE_B = 2.0
STANDUP_DURATION = STANDUP_PHASE_A + STANDUP_PHASE_B

ACTION_SCALE = 0.25

# 站立/趴下姿态 (MuJoCo 腿序: FR, FL, RR, RL; 每腿 ABAD, HIP, KNEE)
STAND_JOINT_POS = np.array([
    0.0, 0.8, -1.5,
    0.0, 0.8, -1.5,
    0.0, 0.8, -1.5,
    0.0, 0.8, -1.5,
])
LIE_JOINT_POS = np.array([
    0.0, 1.4, -2.4,
    0.0, 1.4, -2.4,
    0.0, 1.4, -2.4,
    0.0, 1.4, -2.4,
])

# 策略训练默认位 (运行时反解, 记忆 737932d4; 注意 q_target 与 obs 都用它)
Q_DEFAULT = np.array([
    0.0086, 0.7969, -1.5074,   # FR
    -0.0087, 0.7970, -1.5072,  # FL
    -0.0135, 0.8027, -1.4933,  # RR
    0.0136, 0.8030, -1.4930,   # RL
])

# 速度指令满幅 (m/s, rad/s); obs 缩放 vx*2, vy*2, wz*1 → d9 满幅 6.0
CMD_VX = 3.0
CMD_VY = 1.0
CMD_WZ = 0.75
CMD_RAMP_RATE = 60.0  # Matrix 按 W 后 d9 0→6 约 0.1s


class MatrixPolicyRunner:
    def __init__(self, onnx_path: str, clip_action: bool, d7_const: float):
        self.clip_action = clip_action
        self.d7_const = d7_const

        print(f"[INFO] 加载 Matrix 策略: {onnx_path}")
        self.session = ort.InferenceSession(
            onnx_path, providers=['CPUExecutionProvider'])
        for i in self.session.get_inputs():
            print(f"       in : {i.name} {i.shape}")
        for o in self.session.get_outputs():
            print(f"       out: {o.name} {o.shape}")

        print(f"[INFO] 加载 MuJoCo 模型: {MJCF_PATH}")
        self.model = mujoco.MjModel.from_xml_path(MJCF_PATH)
        self.data = mujoco.MjData(self.model)
        self.steps_per_control = max(1, round(CONTROL_DT / self.model.opt.timestep))
        print(f"       物理步长 {self.model.opt.timestep*1000:.2f}ms, "
              f"控制周期 {CONTROL_DT*1000:.1f}ms, "
              f"每控制步 {self.steps_per_control} 个物理步")

        # LSTM 状态
        self.lstm_h = np.zeros((1, 1, 512), dtype=np.float32)
        self.lstm_c = np.zeros((1, 1, 512), dtype=np.float32)

        # FSM: 'PASSIVE' | 'STANDUP' | 'RL'
        self.fsm_state = 'PASSIVE'
        self.standup_start_time = None
        self._standup_gain = 1.0
        self._standup_from = None  # Phase A 插值起点 (进入 STANDUP 时快照)

        # RL 内部状态
        self.last_actions = np.zeros(12, dtype=np.float32)
        self.vel_cmd = np.zeros(3)          # 当前值 (斜坡后, 物理单位)
        self.vel_cmd_target = np.zeros(3)   # 按键目标值

        # 按键
        self.key_standup = False
        self.key_rl_direct = False
        self.key_liedown = False
        self.keys = {k: False for k in 'wsadqe'}

        self.target_pos = LIE_JOINT_POS.copy()
        self._reset_to_lie()

    # ---------- 状态读取 ----------

    def _reset_to_lie(self):
        self.data.qpos[7:19] = LIE_JOINT_POS
        self.data.qpos[0:3] = [0.0, 0.0, 0.12]
        self.data.qpos[3:7] = [1.0, 0.0, 0.0, 0.0]
        self.data.qvel[:] = 0.0
        mujoco.mj_forward(self.model, self.data)

    def _body_height(self):
        return self.data.qpos[2]

    def _get_obs(self) -> np.ndarray:
        obs = np.zeros(48, dtype=np.float32)
        # [0:2] 恒零
        # [3:5] 本体角速度 (free joint qvel[3:6] 已在本体系)
        obs[3:6] = self.data.qvel[3:6]
        # [6:8] 估计器小量 (默认零, 可配置 d7 常数)
        obs[7] = self.d7_const
        # [9:11] 速度指令 vx*2, vy*2, wz*1
        obs[9] = self.vel_cmd[0] * 2.0
        obs[10] = self.vel_cmd[1] * 2.0
        obs[11] = self.vel_cmd[2] * 1.0
        # [12:23] q - q_default (恒等关节映射, MuJoCo 腿序)
        obs[12:24] = self.data.qpos[7:19] - Q_DEFAULT
        # [24:35] qd
        obs[24:36] = self.data.qvel[6:18]
        # [36:47] 上一步动作 (原始网络输出)
        obs[36:48] = self.last_actions
        return obs

    # ---------- 策略推理 ----------

    def _infer(self):
        obs = self._get_obs()
        feed = {
            'input': obs.reshape(1, -1),
            'h0': self.lstm_h,
            'c0': self.lstm_c,
        }
        out, self.lstm_h, self.lstm_c = self.session.run(
            ['output', 'hn', 'cn'], feed)
        action = out[0].astype(np.float64)
        if not np.all(np.isfinite(action)):
            raise RuntimeError(f"策略输出非有限值: {action}")
        self.last_actions = action.astype(np.float32)

        a = np.clip(action, -1.0, 1.0) if self.clip_action else action
        self.target_pos = Q_DEFAULT + ACTION_SCALE * a

    def reset_rl(self):
        self.lstm_h = np.zeros((1, 1, 512), dtype=np.float32)
        self.lstm_c = np.zeros((1, 1, 512), dtype=np.float32)
        self.last_actions[:] = 0.0
        self.vel_cmd[:] = 0.0
        self.vel_cmd_target[:] = 0.0

    # ---------- 按键 / 指令 ----------

    def _update_vel_cmd(self, dt):
        tgt = np.zeros(3)
        if self.keys['w']: tgt[0] += CMD_VX
        if self.keys['s']: tgt[0] -= CMD_VX
        if self.keys['a']: tgt[1] += CMD_VY
        if self.keys['d']: tgt[1] -= CMD_VY
        if self.keys['q']: tgt[2] += CMD_WZ
        if self.keys['e']: tgt[2] -= CMD_WZ
        self.vel_cmd_target = tgt
        # 一阶斜坡逼近目标 (Matrix: d9 0→6 约 0.1s)
        diff = self.vel_cmd_target - self.vel_cmd
        max_step = CMD_RAMP_RATE * dt
        self.vel_cmd += np.clip(diff, -max_step, max_step)

    # ---------- PD ----------

    def _apply_pd(self, kp, kd):
        tau = (kp * (self.target_pos - self.data.qpos[7:19])
               - kd * self.data.qvel[6:18])
        self.data.ctrl[:] = np.clip(tau, -28.0, 28.0)

    # ---------- FSM ----------

    def _update_fsm(self, dt):
        if self.fsm_state == 'PASSIVE':
            if self.key_standup:
                self.key_standup = False
                self.fsm_state = 'STANDUP'
                self.standup_start_time = self.data.time
                self._standup_from = self.data.qpos[7:19].copy()
                print(f"\n[FSM] PASSIVE -> STANDUP (t={self.data.time:.1f}s)")
            elif self.key_rl_direct:
                self.key_rl_direct = False
                self.reset_rl()
                self.fsm_state = 'RL'
                print(f"\n[FSM] PASSIVE -> RL (趴姿直接进策略, 复刻 Matrix UE 模式) "
                      f"(t={self.data.time:.1f}s)")

        elif self.fsm_state == 'STANDUP':
            elapsed = self.data.time - self.standup_start_time
            if elapsed >= STANDUP_DURATION:
                self.reset_rl()
                self.fsm_state = 'RL'
                print(f"\n[FSM] STANDUP -> RL (t={self.data.time:.1f}s)")

        elif self.fsm_state == 'RL':
            if self.key_liedown:
                self.key_liedown = False
                self.fsm_state = 'PASSIVE'
                self.target_pos = self.data.qpos[7:19].copy()
                self.vel_cmd[:] = 0.0
                self.vel_cmd_target[:] = 0.0
                print(f"\n[FSM] RL -> PASSIVE (t={self.data.time:.1f}s)")
                return
            self._update_vel_cmd(dt)

    def _standup_target(self):
        """Phase A: 插值到趴姿 (归一化起点); Phase B: 趴姿插值到站姿"""
        elapsed = self.data.time - self.standup_start_time
        self._standup_gain = min(elapsed / STANDUP_GAIN_RAMP, 1.0)
        if elapsed < STANDUP_PHASE_A:
            t = elapsed / STANDUP_PHASE_A
        else:
            t = (elapsed - STANDUP_PHASE_A) / STANDUP_PHASE_B
        t = max(0.0, min(t, 1.0))
        t = t * t * (3 - 2 * t)  # smoothstep
        if elapsed < STANDUP_PHASE_A:
            self.target_pos = self._standup_from + t * (
                LIE_JOINT_POS - self._standup_from)
        else:
            self.target_pos = LIE_JOINT_POS + t * (STAND_JOINT_POS - LIE_JOINT_POS)

    # ---------- 主循环 ----------

    def run(self):
        print("\n[控制说明]")
        print("  R 键: 趴姿直接进 RL (复刻 Matrix UE 模式, 策略自己站起)")
        print("  U 键: PD 站立 3s 后进 RL")
        print("  W/S/A/D/Q/E: 速度指令 (满幅 vx=3.0, vy=1.0, wz=0.75)")
        print("  Space: RL 中退回 PASSIVE")
        print("  关闭窗口退出\n")

        with mujoco.viewer.launch_passive(
                self.model, self.data,
                show_left_ui=False, show_right_ui=False) as viewer:
            key_queue = self._start_key_grabber()
            step = 0
            while viewer.is_running():
                t0 = time.time()
                self._process_keys(key_queue)
                dt = CONTROL_DT
                self._update_fsm(dt)

                if self.fsm_state == 'STANDUP':
                    self._standup_target()
                    self._apply_pd(KP_STANDUP * self._standup_gain,
                                   KD_STANDUP * self._standup_gain)
                elif self.fsm_state == 'RL':
                    self._infer()
                    self._apply_pd(KP_RL, KD_RL)
                elif self.fsm_state == 'PASSIVE':
                    self._apply_pd(0.0, 1.0)  # 仅阻尼

                for _ in range(self.steps_per_control):
                    mujoco.mj_step(self.model, self.data)

                viewer.sync()
                step += 1
                if step % 500 == 0:  # 每秒一次
                    h = self._body_height()
                    qrel_knee = self.data.qpos[7:19][2::3] - Q_DEFAULT[2::3]
                    print(f"[t={self.data.time:6.1f}s] FSM={self.fsm_state:<7s} "
                          f"height={h:.3f} "
                          f"cmd=[{self.vel_cmd[0]:+.2f},{self.vel_cmd[1]:+.2f},"
                          f"{self.vel_cmd[2]:+.2f}] "
                          f"act=[{self.last_actions.min():+.2f},"
                          f"{self.last_actions.max():+.2f}] "
                          f"knee_qrel={qrel_knee.mean():+.3f}")

                elapsed = time.time() - t0
                if elapsed < CONTROL_DT:
                    time.sleep(CONTROL_DT - elapsed)

        print("\n[INFO] 仿真结束")

    # ---------- X11 全局按键 ----------

    def _start_key_grabber(self):
        key_queue = queue.Queue()

        def grabber():
            from Xlib import display as x11_display, X as x11_X, XK as x11_XK
            dpy = x11_display.Display()
            root = dpy.screen().root
            grab_names = ['w', 's', 'a', 'd', 'q', 'e', 'u', 'r', 'space']
            keysyms = {n: x11_XK.string_to_keysym(n) for n in grab_names}
            for name in grab_names:
                kc = dpy.keysym_to_keycode(keysyms[name])
                root.grab_key(kc, x11_X.AnyModifier, False,
                              x11_X.GrabModeAsync, x11_X.GrabModeAsync)
            dpy.flush()
            print("[INFO] X11 控制键拦截已启用 (W/S/A/D/Q/E/U/R/Space)")
            while True:
                ev = dpy.next_event()
                if ev.type == x11_X.KeyPress:
                    for n in grab_names:
                        if ev.detail == dpy.keysym_to_keycode(keysyms[n]):
                            key_queue.put(('press', n))
                            break
                elif ev.type == x11_X.KeyRelease:
                    for n in 'wsadqe':
                        if ev.detail == dpy.keysym_to_keycode(keysyms[n]):
                            key_queue.put(('release', n))
                            break

        threading.Thread(target=grabber, daemon=True).start()
        time.sleep(0.3)
        return key_queue

    def _process_keys(self, key_queue):
        while not key_queue.empty():
            try:
                etype, name = key_queue.get_nowait()
            except queue.Empty:
                break
            if etype == 'press':
                if name == 'u':
                    self.key_standup = True
                elif name == 'r':
                    self.key_rl_direct = True
                elif name == 'space':
                    self.key_liedown = True
                elif name in self.keys:
                    self.keys[name] = True
            else:
                if name in self.keys:
                    self.keys[name] = False


def main():
    parser = argparse.ArgumentParser(description="matrix_xgb 快速调试台")
    parser.add_argument('onnx_path', nargs='?', default=DEFAULT_ONNX_PATH,
                        help='policy_mix_walk.onnx 路径')
    parser.add_argument('--no-clip', action='store_true',
                        help='关闭动作 [-1,1] 裁剪 (对比用)')
    parser.add_argument('--d7', type=float, default=0.0,
                        help='obs d6-8 的 d7 常数 (默认 0, 站立平衡点约 0.0401)')
    args = parser.parse_args()

    if not os.path.exists(args.onnx_path):
        print(f"[ERROR] ONNX 模型不存在: {args.onnx_path}")
        sys.exit(1)

    runner = MatrixPolicyRunner(args.onnx_path,
                                clip_action=not args.no_clip,
                                d7_const=args.d7)
    runner.run()


if __name__ == '__main__':
    main()
