set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $dss = $base + 0x2179a0
set $temp = $base + 0x2186d4
set $sun = (char *)calloc(16, 1)
set $sundata = (double *)calloc(3, 8)
set $out = (double *)calloc(14, 8)
set {int}$sun = 3
set {void **}($sun + 8) = $sundata
set $sundata[0] = 1.0
set $sundata[1] = -1.5
set $sundata[2] = 2.0
set $i = 0
while $i < 2
  set $matrix = (double *)calloc(9, 8)
  set $measure = (double *)calloc(3, 8)
  set $item = $dss + $i * 0x158
  set $matrix[0] = 1.0
  set $matrix[4] = 1.0
  set $matrix[8] = 1.0
  set {int}($item + 0x18) = 0
  set {int}($item + 0x58) = 3
  set {int}($item + 0x5c) = 3
  set {int}($item + 0x60) = 3
  set {void **}($item + 0x68) = $matrix
  set {int}($item + 0x110) = 3
  set {void **}($item + 0x118) = $measure
  set $i = $i + 1
end
set {int}$temp = 0
call ((void (*)(void *))UpdateDSS)($sun)
set $i = 0
while $i < 2
  set $item = $dss + $i * 0x158
  set $measureptr = {double **}($item + 0x118)
  set $out[$i * 7 + 0] = $measureptr[0]
  set $out[$i * 7 + 1] = $measureptr[1]
  set $out[$i * 7 + 2] = $measureptr[2]
  set $out[$i * 7 + 3] = {double}($item + 0x138)
  set $out[$i * 7 + 4] = {double}($item + 0x140)
  set $out[$i * 7 + 5] = {double}($item + 0x148)
  set $out[$i * 7 + 6] = {double}($item + 0x150)
  set $i = $i + 1
end
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_dss_p1_state.bin $out $out+14
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_dss_p1_temp.bin $temp $temp+4
quit
