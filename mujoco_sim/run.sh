#!/bin/bash
# mujoco_sim 构建与运行脚本
# 替代 Matrix 的 robot_mujoco

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"
MUJOCO_INSTALL="/home/qiyuan/Softwares/Mujoco330/install"

set -e

# ===== 构建 =====
echo "===== 构建 mujoco_sim ====="
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"
cmake "${SCRIPT_DIR}" \
  -DCMAKE_PREFIX_PATH="${MUJOCO_INSTALL}" \
  -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

echo ""
echo "===== 构建成功 ====="
echo "可执行文件: ${BUILD_DIR}/mujoco_sim"
echo ""

# ===== 运行 =====
echo "===== 启动 mujoco_sim ====="
export LD_LIBRARY_PATH="${MUJOCO_INSTALL}/lib:${LD_LIBRARY_PATH}"

if [ $# -gt 0 ]; then
  "${BUILD_DIR}/mujoco_sim" "$@"
else
  "${BUILD_DIR}/mujoco_sim" "${SCRIPT_DIR}/config.yaml"
fi
