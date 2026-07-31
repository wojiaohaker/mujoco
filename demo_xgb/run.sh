#!/bin/bash
# XGB MuJoCo Demo 构建与运行脚本

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
MUJOCO_INSTALL="/home/qiyuan/Softwares/Mujoco330/install"

set -e

# 构建
echo "===== 构建 demo_xgb ====="
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"
cmake "${SCRIPT_DIR}" \
  -DCMAKE_PREFIX_PATH="${MUJOCO_INSTALL}" \
  -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo ""
echo "===== 构建成功 ====="
echo "可执行文件: ${BUILD_DIR}/demo_xgb"
echo ""

# 运行（设置 LD_LIBRARY_PATH 确保找到 libmujoco.so）
echo "===== 启动 XGB 仿真 ====="
export LD_LIBRARY_PATH="${MUJOCO_INSTALL}/lib:${LD_LIBRARY_PATH}"

if [ $# -gt 0 ]; then
  "${BUILD_DIR}/demo_xgb" "$@"
else
  "${BUILD_DIR}/demo_xgb"
fi
