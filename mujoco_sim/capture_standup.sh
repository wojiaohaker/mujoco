#!/bin/bash
# Matrix 站立轨迹抓取: 以 CAPTURE 模式启动 mujoco_sim
# 用法:
#   1. ./capture_standup.sh          (启动带抓取的 mujoco_sim)
#   2. 另开终端跑 mc_ctrl, 按 U 站立, 等站稳后 Ctrl+C 退出 mc_ctrl
#   3. Ctrl+C 退出本脚本的 mujoco_sim
#   4. python3 reconstruct_standup.py  → 输出路点表
set -e
cd "$(dirname "$0")"
rm -f /tmp/standup_capture.csv
echo "===== CAPTURE 模式启动 mujoco_sim ====="
echo "轨迹将写入 /tmp/standup_capture.csv (100Hz, 全 12 关节 q/qdes)"
echo ""
CAPTURE=1 ./build/mujoco_sim config.yaml
