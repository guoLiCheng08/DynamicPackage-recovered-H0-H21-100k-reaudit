#!/usr/bin/env bash
set -u -o pipefail

cd "$(dirname "$0")/.."
report_dir="${1:-build/validation_reports/matrix_$(date +%Y%m%d_%H%M%S)}"
scenario_dir="$report_dir/scenarios"
mkdir -p "$report_dir"
python3 tools/generate_shadow_scenarios.py "$scenario_dir" >/dev/null

failed=0
printf '编号\t工况\t步数\t结果\t报告目录\n' >"$report_dir/summary.tsv"
for scenario in "$scenario_dir"/*.csv; do
    name="$(basename "$scenario" .csv)"
    steps="$(awk '/^[[:space:]]*($|#)/ { next } { ++count } END { print count + 0 }' "$scenario")"
    case_dir="$report_dir/$name"
    if tools/run_strict_independent_validation.sh "$steps" "$case_dir" "$scenario"; then
        result="通过"
    else
        result="失败"
        failed=1
    fi
    printf '%s\t%s\t%s\t%s\t%s\n' "${name%%_*}" "$name" "$steps" "$result" "$case_dir" \
        >>"$report_dir/summary.tsv"
done

column -ts $'\t' "$report_dir/summary.tsv" 2>/dev/null || cat "$report_dir/summary.tsv"
exit "$failed"
