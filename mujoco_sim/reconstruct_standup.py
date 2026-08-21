#!/usr/bin/env python3
"""反推 mc_ctrl 站立轨迹的关节路点。

用法: python3 reconstruct_standup.py [/tmp/standup_capture.csv]

流程:
1. 读取 CAPTURE=1 模式下 mujoco_sim 记录的高频轨迹 (100Hz)
2. 自动定位站立窗口 (base z 从 <0.15 升到 >0.25)
3. 对 12 个关节的 qdes(t) 做 Douglas-Peucker 分段线性拟合, 反推路点
4. 输出路点表 (可直接抄进 qiyuan_mc STANDUP 的插值序列)
"""

import sys

CAP_FILE = sys.argv[1] if len(sys.argv) > 1 else "/tmp/standup_capture.csv"
EPS = 0.015  # 分段拟合容差 [rad], 低于此偏差视为直线段

LEGS = ["FR", "FL", "RL", "RR"]
# CSV 列: step time x y z qw qx qy qz + 每腿 (ab_q ab_d hi_q hi_d kn_q kn_d)*4
# qdes 列索引 (0 基): 每腿首列 ab_q 在 9+li*6, qdes 在 +1/+3/+5
QDES_COLS = []
NAMES = []
for li, leg in enumerate(LEGS):
    base = 9 + li * 6
    for jt, name in [(1, "ab"), (3, "hi"), (5, "kn")]:
        QDES_COLS.append(base + jt)
        NAMES.append(f"{leg}_{name}")


def load(path):
    t, z, qw, qx, qy, qdes = [], [], [], [], [], []
    for ln in open(path):
        if not ln.strip() or ln.startswith("#"):
            continue
        f = ln.split()
        if len(f) < 33:
            continue
        t.append(float(f[1]))
        z.append(float(f[4]))
        qw.append(float(f[5]))
        qx.append(float(f[6]))
        qy.append(float(f[7]))
        qdes.append([float(f[c]) for c in QDES_COLS])
    return t, z, qw, qx, qy, qdes


def find_stand_window(t, z):
    """站立窗口: 找到 z 最后一次从低位(<0.12)升到高位(>0.26) 的区间。"""
    lo = hi = None
    for i in range(len(z)):
        if z[i] < 0.12:
            lo = i  # 不断刷新, 保留最后一个低点
        elif lo is not None and z[i] > 0.26 and hi is None:
            hi = i
    if lo is None or hi is None:
        # 退化: 找 z 上升最快的区间
        return 0, len(t) - 1
    # 窗口前后各扩 0.5s 以覆盖收腿阶段
    i0 = lo
    while i0 > 0 and t[lo] - t[i0] < 1.5:
        i0 -= 1
    i1 = hi
    while i1 < len(t) - 1 and t[i1] - t[hi] < 1.0:
        i1 += 1
    return i0, i1


def douglas_peucker(pts, eps):
    """pts: [(t, v)]; 返回保留的关键点索引。"""
    keep = [0, len(pts) - 1]

    def recurse(a, b):
        if b - a < 2:
            return
        (t0, v0), (t1, v1) = pts[a], pts[b]
        dmax, imax = -1.0, -1
        for i in range(a + 1, b):
            tt, vv = pts[i]
            if t1 == t0:
                d = abs(vv - v0)
            else:
                pred = v0 + (v1 - v0) * (tt - t0) / (t1 - t0)
                d = abs(vv - pred)
            if d > dmax:
                dmax, imax = d, i
        if dmax > eps:
            keep.append(imax)
            recurse(a, imax)
            recurse(imax, b)

    recurse(0, len(pts) - 1)
    return sorted(keep)


def main():
    t, z, qw, qx, qy, qdes = load(CAP_FILE)
    if len(t) < 10:
        print(f"[错误] {CAP_FILE} 数据不足 ({len(t)} 行)。"
              f"请确认 mujoco_sim 以 CAPTURE=1 启动。")
        return

    i0, i1 = find_stand_window(t, z)
    print(f"数据: {len(t)} 帧, t={t[0]:.2f}~{t[-1]:.2f}s, 100Hz")
    print(f"站立窗口: t={t[i0]:.2f} ~ {t[i1]:.2f}s (h: {z[i0]:.3f} -> {z[i1]:.3f})")
    print(f"窗口内姿态: roll={2*max(abs(x) for x in qx[i0:i1+1]):.3f} "
          f"pitch={2*max(abs(y) for y in qy[i0:i1+1]):.3f} (rad, 小角度近似)")
    print()

    t0 = t[i0]
    print("===== 各关节 qdes 路点 (t 相对窗口起点) =====")
    for j, name in enumerate(NAMES):
        pts = [(t[i] - t0, qdes[i][j]) for i in range(i0, i1 + 1)]
        # 去掉静止首尾 (值完全不变的帧压缩)
        idx = douglas_peucker(pts, EPS)
        wps = [(pts[i][0], pts[i][1]) for i in idx]
        # 合并过近的路点
        merged = [wps[0]]
        for tt, vv in wps[1:]:
            if abs(vv - merged[-1][1]) > 1e-4 or tt - merged[-1][0] > 0.05:
                merged.append((tt, vv))
        seg = "  ".join(f"({tt:+.2f}s,{vv:+.3f})" for tt, vv in merged)
        print(f"{name:>6}: {seg}")

    print()
    print("===== 身高轨迹 (0.2s 采样) =====")
    for i in range(i0, i1 + 1, 20):
        print(f"  t={t[i]-t0:+.2f}s  h={z[i]:.3f}")
    print(f"  t={t[i1]-t0:+.2f}s  h={z[i1]:.3f}")


if __name__ == "__main__":
    main()
