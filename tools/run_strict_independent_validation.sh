#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."
steps="${1:-60000}"
case_dir="${2:-build/validation_reports/independent_$(date +%Y%m%d_%H%M%S)}"
input_csv="${3:-}"
replay_args=("$steps")
if [[ -n "$input_csv" ]]; then
    steps="$(awk 'BEGIN { count=0 } /^[[:space:]]*($|#)/ { next } { ++count } END { print count }' "$input_csv")"
    [[ "$steps" -gt 0 ]]
    replay_args=(--csv "$input_csv")
fi
runtime_steps="$steps"
# 回放器默认会先发送 2 个零输入预置帧，用于完成启动态收敛；两路进程
# 必须为这两帧预留积分步，否则正式帧结束后会等待不存在的序号。
runtime_steps=$((steps + 2))
mkdir -p "$case_dir"

release="$case_dir/start.release"
arm="$case_dir/rng.arm"
rng="$case_dir/rng.bin"
rng_callers="$case_dir/rng_callers.txt"
rng_gaussian_callers="$case_dir/rng_gaussian_callers.txt"
rng_gaussian_parents="$case_dir/rng_gaussian_parents.txt"
c_gaussian_callers="$case_dir/c_gaussian_callers.txt"
c_rng_stages="$case_dir/c_rng_stages.txt"
elf_rhs="$case_dir/elf_rhs.bin"
c_rhs="$case_dir/c_rhs.bin"
elf_rhs_context="$case_dir/elf_rhs_context.bin"
c_rhs_context="$case_dir/c_rhs_context.bin"
elf_commands="$case_dir/elf_commands.txt"
input_delivery="$case_dir/input_delivery.txt"
ready="$case_dir/trace.ready"
c_ready="$case_dir/c_shadow.ready"
state_shm="/cfs_test_elf_state_$$"
c_state_shm="/cfs_test_c_shadow_state_$$"
input_shm="/cfs_test_shadow_input_$$"
c_ipc_shm="/cfs_test_c_shadow_ipc_$$"
c_pid=""
elf_pid=""
trace_pid=""
c_trace_pid=""
c_runtime_bin="${C_SHADOW_RUNTIME_BIN:-./build/c_shadow_runtime}"

cleanup()
{
    [[ -n "$trace_pid" ]] && kill "$trace_pid" 2>/dev/null || true
    [[ -n "$c_trace_pid" ]] && kill "$c_trace_pid" 2>/dev/null || true
    [[ -n "$c_pid" ]] && kill "$c_pid" 2>/dev/null || true
    [[ -n "$elf_pid" ]] && kill "$elf_pid" 2>/dev/null || true
    [[ -n "$trace_pid" ]] && wait "$trace_pid" 2>/dev/null || true
    [[ -n "$c_trace_pid" ]] && wait "$c_trace_pid" 2>/dev/null || true
    [[ -n "$c_pid" ]] && wait "$c_pid" 2>/dev/null || true
    [[ -n "$elf_pid" ]] && wait "$elf_pid" 2>/dev/null || true
    rm -f "/dev/shm/${input_shm#/}"
    rm -f "/dev/shm/${c_state_shm#/}"
    rm -f "/dev/shm/${state_shm#/}"
    rm -f "/dev/shm/${c_ipc_shm#/}"
}
trap cleanup EXIT

make shadow-runtime trace-elf-dyn-main trace-c-rhs run-dual-sequence compare-state-logs \
    compare-rhs-context \
    rng-record start-gate >/dev/null
touch "$rng"

C_SHADOW_REPLAY_REQUIRED=1 \
C_SHADOW_INPUT_SHM="$input_shm" \
C_SHADOW_STATE_SHM="$c_state_shm" \
C_SHADOW_GAUSSIAN_TRACE="$c_gaussian_callers" \
C_SHADOW_RNG_STAGE_TRACE="$c_rng_stages" \
C_SHADOW_SEED_FROM_ELF=1 \
C_SHADOW_STRICT_INDEPENDENT=1 \
C_SHADOW_RNG_REPLAY="$rng" \
C_SHADOW_IPC_SHM="$c_ipc_shm" \
ELF_C_SHADOW_STATE_SHM="$state_shm" \
C_SHADOW_READY_FILE="$c_ready" \
C_SHADOW_STATE_LOG="$case_dir/c_state.bin" \
C_SHADOW_STARTUP_PRIMER=2 \
C_SHADOW_RHS_CONTEXT_TRACE="${C_SHADOW_TRACE_RHS_CONTEXT:+$c_rhs_context}" \
C_SHADOW_RHS_CONTEXT_SEQUENCE="${C_SHADOW_RHS_SEQUENCE:-1986}" \
"$c_runtime_bin" "$runtime_steps" >"$case_dir/c_runtime.log" 2>&1 &
c_pid=$!

for _ in $(seq 1 3000); do
    [[ -e "/dev/shm/${input_shm#/}" ]] && break
    sleep 0.01
done
[[ -e "/dev/shm/${input_shm#/}" ]]

LD_PRELOAD="$PWD/build/libdp_start_gate.so:$PWD/build/libdp_rng_record.so" \
DP_START_GATE_RELEASE_FILE="$release" \
DP_RNG_ARM_FILE="$arm" \
DP_RNG_RECORD_FILE="$rng" \
DP_RNG_CALLER_RECORD_FILE="$rng_callers" \
DP_RNG_GAUSSIAN_CALLER_RECORD_FILE="$rng_gaussian_callers" \
DP_RNG_GAUSSIAN_PARENT_RECORD_FILE="$rng_gaussian_parents" \
/home/gpc22/code/cfs_test/tools/DynamicPackage >"$case_dir/elf_runtime.log" 2>&1 &
elf_pid=$!

# 等待 exec 已完成、ELF 的可执行映射已出现。否则 ptrace 可能附加到尚未
# exec 的 shell 子进程，采样器无法按 DynamicPackage 的映射计算基址。
for _ in $(seq 1 3000); do
    [[ -r "/proc/$elf_pid/maps" ]] &&
        awk '$2 ~ /r.xp/ && $NF ~ /\/DynamicPackage$/ { found=1 } END { exit !found }' \
            "/proc/$elf_pid/maps" && break
    sleep 0.01
done
[[ -r "/proc/$elf_pid/maps" ]]
awk '$2 ~ /r.xp/ && $NF ~ /\/DynamicPackage$/ { found=1 } END { exit !found }' \
    "/proc/$elf_pid/maps"

ELF_C_SHADOW_STATE_SHM="$state_shm" \
C_SHADOW_INPUT_SHM="$input_shm" \
C_SHADOW_STATE_SHM="$c_state_shm" \
ELF_RNG_ARM_FILE="$arm" \
ELF_COMMAND_TRACE_FILE="$elf_commands" \
DP_RNG_RECORD_FILE="$rng" \
ELF_TRACE_REQUIRE_INITIALIZED=1 \
ELF_STATE_LOG="$case_dir/elf_state.bin" \
ELF_SEED_SNAPSHOT_FILE="$case_dir/elf_seed.bin" \
ELF_TRACE_READY_FILE="$ready" \
ELF_TRACE_DISABLE_POST_ACK=1 \
ELF_RHS_TRACE_FILE="$elf_rhs" \
ELF_RHS_TRACE_SEQUENCE="${C_SHADOW_RHS_SEQUENCE:-1986}" \
ELF_RHS_CONTEXT_TRACE_FILE="${C_SHADOW_TRACE_RHS_CONTEXT:+$elf_rhs_context}" \
ELF_RHS_CONTEXT_SEQUENCE="${C_SHADOW_RHS_SEQUENCE:-1986}" \
./build/trace_elf_dyn_main "$elf_pid" "$runtime_steps" 1 >"$case_dir/elf_trace.log" 2>&1 &
trace_pid=$!

for _ in $(seq 1 3000); do
    [[ -f "$ready" ]] && break
    sleep 0.01
done
[[ -f "$ready" ]]
touch "$release"

for _ in $(seq 1 3000); do
    [[ -f "$c_ready" ]] && break
    # 短测可能在轮询间隔内完成并删除 ready 文件；此时由后续 wait
    # 检查退出状态，不能把正常完成误判为同步失败。
    if [[ -n "$c_pid" ]] && ! kill -0 "$c_pid" 2>/dev/null; then
        break
    fi
    sleep 0.01
done
if [[ ! -f "$c_ready" ]] && [[ -n "$c_pid" ]] && kill -0 "$c_pid" 2>/dev/null; then
    echo "纯C影子未完成ready握手" >&2
    exit 5
fi

if [[ "${C_SHADOW_TRACE_C_RHS:-0}" == "1" ]]; then
    c_rhs_offset="$(nm -n build/c_shadow_runtime | awk '$3 == "differential_equation" { value="0x" $1 } END { print value }')"
    [[ -n "$c_rhs_offset" ]]
    ./build/trace_c_rhs "$c_pid" "$c_rhs_offset" "${C_SHADOW_RHS_SEQUENCE:-1986}" "$c_rhs" \
        >"$case_dir/c_rhs_trace.log" 2>&1 &
    c_trace_pid=$!
    for _ in $(seq 1 3000); do
        tracer_pid="$(awk '/TracerPid:/ { print $2 }' "/proc/$c_pid/status")"
        [[ "$tracer_pid" == "$c_trace_pid" ]] && break
        sleep 0.01
    done
    [[ "$tracer_pid" == "$c_trace_pid" ]]
fi

for _ in $(seq 1 3000); do
    [[ -e "/proc/$elf_pid/fd/3" ]] && break
    sleep 0.01
done
[[ -e "/proc/$elf_pid/fd/3" ]]

ELF_INPUT_FD="/proc/$elf_pid/fd/3" \
C_SHADOW_INPUT_SHM="$input_shm" \
C_SHADOW_STATE_SHM="$c_state_shm" \
ELF_C_SHADOW_STATE_SHM="$state_shm" \
INPUT_DELIVERY_LOG="$input_delivery" \
C_SHADOW_STARTUP_PRIMER=2 \
./build/run_dual_sequence "${replay_args[@]}" >"$case_dir/input_replay.log" 2>&1

wait "$c_pid"
c_pid=""
if [[ -n "$c_trace_pid" ]]; then
    wait "$c_trace_pid"
    c_trace_pid=""
fi
wait "$trace_pid"
trace_pid=""
if [[ "${C_SHADOW_TRACE_RHS_CONTEXT:-0}" == "1" ]]; then
    ./build/compare_rhs_context "$c_rhs_context" "$elf_rhs_context" \
        >"$case_dir/rhs_context_compare.log" 2>&1 || true
fi
compare_status=0
if ! ./build/compare_state_logs "$case_dir/c_state.bin" "$case_dir/elf_state.bin" \
    >"$case_dir/compare.log" 2>&1; then
    compare_status=1
fi

{
    if [[ "$compare_status" == "0" ]]; then
        echo "1. 结果=通过"
    else
        echo "1. 结果=失败"
    fi
    echo "2. 模式=严格独立双路回放"
    echo "3. 积分步数=$steps"
    awk -F= '/仿真时长=/{print "4. 仿真时长=" $2}' "$case_dir/input_replay.log"
    awk -F= '/输入交付不一致=/{print "5. 输入交付不一致=" $2}' "$case_dir/input_replay.log"
    awk -F= '/^1\. 结果=/{print "6. 比较结果=" $2} \
               /^2\. 逐步比较=/{print "7. 逐步比较=" $2} \
               /^3\. 失败行=/{print "8. 失败行=" $2}' "$case_dir/compare.log"
    echo "9. 说明=C 仅在首步使用 ELF 初态；后续时间、状态、设备和遥测均由 C 自行推进。"
    echo "10. 输入原则=C 与 ELF 使用同一份 ELF 入口实际控制结构；请求/实际不一致单列，不计入模型通过。"
} >"$case_dir/report.txt"

cat "$case_dir/report.txt"
echo "报告目录=$case_dir"
exit "$compare_status"
