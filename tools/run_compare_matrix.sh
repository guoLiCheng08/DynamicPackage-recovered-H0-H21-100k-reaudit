#!/usr/bin/env bash
set -u

report_dir=${1:-build/compare_reports}
mkdir -p "$report_dir"
report="$report_dir/summary.tsv"
printf 'suite\tstatus\tfirst_evidence\tlog\n' > "$report"

suites=(
  check-h0-fifty check-h15-global-y check-h16-h17-global-y
  check-h4-h5-h7-global-y check-h26-high-ecc-cross-command
  check-h27-third-seed-cross-command check-h28-dyn-main-array
  check-h29-get-desk-command check-h30-startup-chain
  check-h31-get-desk-lifecycle check-h32-high-ecc-safe-boundary
  check-h33-ipc-getter-invalid check-h34-circular-equatorial
  check-h35-near-parabolic
)

for suite in "${suites[@]}"; do
  log="$report_dir/$suite.log"
  make "$suite" >"$log" 2>&1
  rc=$?
  if grep -q 'cannot read\|No such file or directory' "$log"; then
    status=missing_gold
    evidence=$(grep -m1 -E 'cannot read|No such file or directory' "$log" | tr '\t' ' ')
  elif grep -q 'FAIL' "$log" || [ "$rc" -ne 0 ]; then
    status=fail
    evidence=$(grep -m1 -E 'FAIL|mismatch|Error' "$log" | tr '\t' ' ')
  else
    status=pass
    evidence=$(grep -m1 -E 'PASS|passed' "$log" | tr '\t' ' ')
  fi
  printf '%s\t%s\t%s\t%s\n' "$suite" "$status" "$evidence" "$log" >> "$report"
  printf '%-40s %s\n' "$suite" "$status"
done

printf 'report=%s\n' "$report"
