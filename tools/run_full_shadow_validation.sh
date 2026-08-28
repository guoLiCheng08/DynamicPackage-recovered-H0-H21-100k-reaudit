#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."
mkdir -p build/validation_reports
report="build/validation_reports/shadow_validation_$(date +%Y%m%d_%H%M%S).tsv"
overall_rc=0

make shadow-runtime compare-state >/tmp/c_shadow_validation_build.log 2>&1
printf '项目\t步数\t结果\t实际\t说明\n' >"$report"

for steps in 100 1000 10000; do
    log="build/validation_reports/c_shadow_${steps}.log"
    if ./build/c_shadow_runtime "$steps" >"$log" 2>&1; then
        actual=$(awk -F= '/执行步数=/{print $2; exit}' "$log")
        printf '纯C独立运行\t%s\t通过\t%s\t33维状态、设备状态、完整遥测均已生成\n' \
               "$steps" "${actual:-未知}" >>"$report"
    else
        printf '纯C独立运行\t%s\t失败\t-\t查看%s\n' "$steps" "$log" >>"$report"
        overall_rc=1
    fi
done

if [[ -n "${ELF_PID:-}" ]]; then
    ready_file="/tmp/cfs_test_c_shadow_validation.$$.ready"
    C_SHADOW_READY_FILE="$ready_file" ./build/c_shadow_runtime 0 \
        >"build/validation_reports/c_shadow_live.log" 2>&1 &
    shadow_pid=$!
    trap 'kill -TERM "$shadow_pid" 2>/dev/null || true; wait "$shadow_pid" 2>/dev/null || true' EXIT
    for _ in $(seq 1 50); do
        [[ -f "$ready_file" ]] && break
        sleep 0.02
    done
    state_log="build/validation_reports/elf_c_state.log"
    if [[ ! -f "$ready_file" ]]; then
        printf 'ELF与纯C状态\t当前\t失败\t-\t纯C影子进程未就绪，详见build/validation_reports/c_shadow_live.log\n' >>"$report"
        overall_rc=1
    elif C_SHADOW_STATE_SHM="${C_SHADOW_STATE_SHM:-/cfs_test_c_shadow_state}" \
         ./build/compare_elf_c_state "$ELF_PID" >"$state_log" 2>&1; then
        printf 'ELF与纯C状态\t当前\t通过\t33/33\t正式ELF y[33]与纯C状态帧一致\n' >>"$report"
    else
        printf 'ELF与纯C状态\t当前\t失败\t-\t%s\n' "$state_log" >>"$report"
        overall_rc=1
    fi
else
    printf 'ELF与纯C状态\t当前\t未执行\t-\t设置 ELF_PID=正式ELF进程号后执行\n' >>"$report"
fi

printf '报告=%s\n' "$report"
column -ts $'\t' "$report" 2>/dev/null || cat "$report"
exit "$overall_rc"
