#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."
steps="${1:-60000}"
case_dir="${2:-build/validation_reports/independent_$(date +%Y%m%d_%H%M%S)}"
mkdir -p "$case_dir"

release="$case_dir/start.release"
arm="$case_dir/rng.arm"
rng="$case_dir/rng.bin"
ready="$case_dir/trace.ready"
state_shm="/cfs_test_elf_state_$$"
c_pid=""
elf_pid=""
trace_pid=""

cleanup()
{
    [[ -n "$trace_pid" ]] && kill "$trace_pid" 2>/dev/null || true
    [[ -n "$c_pid" ]] && kill "$c_pid" 2>/dev/null || true
    [[ -n "$elf_pid" ]] && kill "$elf_pid" 2>/dev/null || true
    [[ -n "$trace_pid" ]] && wait "$trace_pid" 2>/dev/null || true
    [[ -n "$c_pid" ]] && wait "$c_pid" 2>/dev/null || true
    [[ -n "$elf_pid" ]] && wait "$elf_pid" 2>/dev/null || true
}
trap cleanup EXIT

make shadow-runtime trace-elf-dyn-main run-dual-sequence compare-state-logs \
    rng-record start-gate >/dev/null
touch "$rng"

C_SHADOW_REPLAY_REQUIRED=1 \
C_SHADOW_SEED_FROM_ELF=1 \
C_SHADOW_STRICT_INDEPENDENT=1 \
C_SHADOW_RNG_REPLAY="$rng" \
ELF_C_SHADOW_STATE_SHM="$state_shm" \
C_SHADOW_STATE_LOG="$case_dir/c_state.bin" \
./build/c_shadow_runtime "$steps" >"$case_dir/c_runtime.log" 2>&1 &
c_pid=$!

LD_PRELOAD="$PWD/build/libdp_start_gate.so:$PWD/build/libdp_rng_record.so" \
DP_START_GATE_RELEASE_FILE="$release" \
DP_RNG_ARM_FILE="$arm" \
DP_RNG_RECORD_FILE="$rng" \
/home/gpc22/code/cfs_test/tools/DynamicPackage >"$case_dir/elf_runtime.log" 2>&1 &
elf_pid=$!

ELF_C_SHADOW_STATE_SHM="$state_shm" \
ELF_RNG_ARM_FILE="$arm" \
ELF_STATE_LOG="$case_dir/elf_state.bin" \
ELF_TRACE_READY_FILE="$ready" \
ELF_TRACE_DISABLE_POST_ACK=1 \
./build/trace_elf_dyn_main "$elf_pid" "$steps" 1 >"$case_dir/elf_trace.log" 2>&1 &
trace_pid=$!

for _ in $(seq 1 3000); do
    [[ -f "$ready" ]] && break
    sleep 0.01
done
[[ -f "$ready" ]]
touch "$release"

for _ in $(seq 1 3000); do
    [[ -e "/proc/$elf_pid/fd/3" ]] && break
    sleep 0.01
done
[[ -e "/proc/$elf_pid/fd/3" ]]

ELF_INPUT_FD="/proc/$elf_pid/fd/3" \
ELF_C_SHADOW_STATE_SHM="$state_shm" \
./build/run_dual_sequence "$steps" >"$case_dir/input_replay.log" 2>&1

wait "$c_pid"
c_pid=""
wait "$trace_pid"
trace_pid=""
./build/compare_state_logs "$case_dir/elf_state.bin" "$case_dir/c_state.bin" \
    >"$case_dir/compare.log" 2>&1

{
    echo "1. 结果=通过"
    echo "2. 模式=严格独立双路回放"
    echo "3. 积分步数=$steps"
    awk -F= '/仿真时长=/{print "4. 仿真时长=" $2}' "$case_dir/input_replay.log"
    awk -F= 'NR == 1 {print "5. 比较结果=" $2} NR == 2 {print "6. 逐步比较=" $2} NR == 3 {print "7. 失败行=" $2}' \
        "$case_dir/compare.log"
    echo "8. 说明=C 仅在首步使用 ELF 初态，后续时间、状态、设备和遥测均由 C 自行推进。"
} >"$case_dir/report.txt"

cat "$case_dir/report.txt"
echo "报告目录=$case_dir"
