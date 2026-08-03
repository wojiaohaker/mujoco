#!/usr/bin/env bash
# =============================================================================
# MuJoCo + CarlaUnreal + mc_ctrl 联合仿真启动脚本
#
# 架构:
#   mujoco_sim (物理仿真, 500Hz)
#       ├── eCAL mujoco_state ──→ mc_ctrl (RL运动控制)
#       ├── eCAL mujoco_cmd  ←── mc_ctrl
#       ├── UDP 25001 ──────────→ mc_ctrl (备用)
#       └── UDP 25003 ──────────→ CarlaUnreal (纯渲染)
#
# 用法:
#   ./run_cosim.sh          # 启动全部三个进程
#   ./run_cosim.sh stop     # 停止全部
#   ./run_cosim.sh status   # 查看状态
# =============================================================================

set -euo pipefail

# ===== 路径配置 =====
MUJOCO_SIM_DIR="/home/qiyuan/Softwares/Mujoco330/mujoco_sim"
MUJOCO_SIM_BIN="${MUJOCO_SIM_DIR}/build/mujoco_sim"
MUJOCO_SIM_CFG="${MUJOCO_SIM_DIR}/config.yaml"

MC_CTRL_DIR="/home/qiyuan/Softwares/Matrix/src/robot_mc/build/export/mc/bin"
MC_CTRL_BIN="${MC_CTRL_DIR}/mc_ctrl"

CARLA_DIR="/home/qiyuan/UnrealEngine/CarlaUE5/Unreal/CarlaUnreal"
CARLA_BIN="${CARLA_DIR}/Binaries/Linux/CarlaUnreal"

# ===== 日志目录 =====
LOG_DIR="/tmp/cosim_logs"
mkdir -p "${LOG_DIR}"

# ===== PID 文件 =====
PID_FILE="/tmp/cosim_pids"

# ===== 颜色输出 =====
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info()  { echo -e "${GREEN}[INFO]${NC} $*"; }
log_warn()  { echo -e "${YELLOW}[WARN]${NC} $*"; }
log_error() { echo -e "${RED}[ERROR]${NC} $*"; }

# ===== 停止函数 =====
stop_all() {
    log_info "停止联合仿真..."
    if [[ -f "${PID_FILE}" ]]; then
        while read -r pid name; do
            if kill -0 "${pid}" 2>/dev/null; then
                log_info "  停止 ${name} (PID=${pid})"
                kill "${pid}" 2>/dev/null || true
            fi
        done < "${PID_FILE}"
        rm -f "${PID_FILE}"
    fi
    # 额外清理（按进程名）
    pkill -f "mujoco_sim" 2>/dev/null || true
    pkill -f "mc_ctrl" 2>/dev/null || true
    # 不自动杀 CarlaUnreal（可能用户还在用）
    log_info "完成。"
}

# ===== 状态函数 =====
show_status() {
    echo "===== 联合仿真状态 ====="
    echo ""
    echo "mujoco_sim:"
    pgrep -a mujoco_sim 2>/dev/null || echo "  未运行"
    echo ""
    echo "mc_ctrl:"
    pgrep -a mc_ctrl 2>/dev/null || echo "  未运行"
    echo ""
    echo "CarlaUnreal:"
    pgrep -a CarlaUnreal 2>/dev/null || echo "  未运行"
    echo ""
    echo "UDP 端口监听:"
    ss -ulnp 2>/dev/null | grep -E "25001|25002|25003" || echo "  无"
    echo ""
    echo "eCAL topics:"
    # eCAL monitor (如果可用)
    if command -v ecal_monitor &>/dev/null; then
        timeout 2 ecal_monitor 2>/dev/null || echo "  (需要 ecal_monitor)"
    else
        echo "  (ecal_monitor 不可用)"
    fi
}

# ===== 主启动流程 =====
start_all() {
    log_info "=========================================="
    log_info "MuJoCo + CarlaUnreal 联合仿真启动"
    log_info "=========================================="

    # 清理旧进程
    stop_all 2>/dev/null || true
    sleep 1

    # ----- 1. 启动 mujoco_sim -----
    log_info "[1/3] 启动 mujoco_sim..."
    if [[ ! -x "${MUJOCO_SIM_BIN}" ]]; then
        log_error "mujoco_sim 不存在: ${MUJOCO_SIM_BIN}"
        log_info "请先编译: cd ${MUJOCO_SIM_DIR}/build && cmake .. && make"
        exit 1
    fi

    cd "${MUJOCO_SIM_DIR}"
    "${MUJOCO_SIM_BIN}" "${MUJOCO_SIM_CFG}" \
        > "${LOG_DIR}/mujoco_sim.log" 2>&1 &
    MUJOCO_PID=$!
    log_info "  mujoco_sim started (PID=${MUJOCO_PID})"
    log_info "  日志: ${LOG_DIR}/mujoco_sim.log"

    # 等待 mujoco_sim 初始化
    sleep 2

    # 验证 mujoco_sim 是否存活
    if ! kill -0 "${MUJOCO_PID}" 2>/dev/null; then
        log_error "mujoco_sim 启动失败！查看日志:"
        tail -20 "${LOG_DIR}/mujoco_sim.log"
        exit 1
    fi

    # ----- 2. 启动 mc_ctrl -----
    log_info "[2/3] 启动 mc_ctrl..."
    if [[ ! -x "${MC_CTRL_BIN}" ]]; then
        log_error "mc_ctrl 不存在: ${MC_CTRL_BIN}"
        exit 1
    fi

    cd "${MC_CTRL_DIR}"
    export LD_LIBRARY_PATH="${MC_CTRL_DIR}:${LD_LIBRARY_PATH:-}"
    export ROBOT_TYPE=XG
    taskset -c 7 "${MC_CTRL_BIN}" r \
        > "${LOG_DIR}/mc_ctrl.log" 2>&1 &
    MC_PID=$!
    log_info "  mc_ctrl started (PID=${MC_PID})"
    log_info "  日志: ${LOG_DIR}/mc_ctrl.log"

    # 等待 mc_ctrl 初始化
    sleep 2

    # ----- 3. 启动 CarlaUnreal -----
    log_info "[3/3] 启动 CarlaUnreal (外部物理模式)..."
    if [[ ! -x "${CARLA_BIN}" ]]; then
        log_warn "CarlaUnreal 不存在: ${CARLA_BIN}"
        log_warn "跳过 CarlaUnreal 启动（可手动启动）"
        CARLA_PID=""
    else
        cd "${CARLA_DIR}"
        "${CARLA_BIN}" -game -log \
            > "${LOG_DIR}/carlaunreal.log" 2>&1 &
        CARLA_PID=$!
        log_info "  CarlaUnreal started (PID=${CARLA_PID})"
        log_info "  日志: ${LOG_DIR}/carlaunreal.log"
    fi

    # ----- 保存 PID -----
    echo "${MUJOCO_PID} mujoco_sim" > "${PID_FILE}"
    echo "${MC_PID} mc_ctrl" >> "${PID_FILE}"
    [[ -n "${CARLA_PID:-}" ]] && echo "${CARLA_PID} CarlaUnreal" >> "${PID_FILE}"

    # ----- 完成 -----
    echo ""
    log_info "=========================================="
    log_info "联合仿真已启动！"
    log_info "=========================================="
    echo ""
    echo "进程状态:"
    echo "  mujoco_sim  : PID=${MUJOCO_PID}"
    echo "  mc_ctrl     : PID=${MC_PID}"
    [[ -n "${CARLA_PID:-}" ]] && echo "  CarlaUnreal : PID=${CARLA_PID}"
    echo ""
    echo "日志文件:"
    echo "  ${LOG_DIR}/mujoco_sim.log"
    echo "  ${LOG_DIR}/mc_ctrl.log"
    [[ -n "${CARLA_PID:-}" ]] && echo "  ${LOG_DIR}/carlaunreal.log"
    echo ""
    echo "控制命令:"
    echo "  $0 stop     # 停止全部"
    echo "  $0 status   # 查看状态"
    echo ""
    echo "按 Ctrl+C 停止全部进程..."

    # 等待并处理退出
    trap "stop_all; exit 0" INT TERM
    wait
}

# ===== 入口 =====
case "${1:-start}" in
    start)
        start_all
        ;;
    stop)
        stop_all
        ;;
    status)
        show_status
        ;;
    *)
        echo "用法: $0 {start|stop|status}"
        exit 1
        ;;
esac
