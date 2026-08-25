set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $mtq = $base + 0x217160
set $cmd = (double *)calloc(6, 8)
set $out = (double *)calloc(6, 8)
set $cmd[0] = 0.5
set $cmd[1] = 3.0
set $cmd[2] = -4.0
set $cmd[3] = 0.0
set $cmd[4] = -1.5
set $cmd[5] = 2.0
set {double}($mtq + 0 * 0x38 + 0x08) = 1.0
set {double}($mtq + 1 * 0x38 + 0x08) = 2.0
set {double}($mtq + 2 * 0x38 + 0x08) = 3.0
set {double}($mtq + 3 * 0x38 + 0x08) = 4.0
set {double}($mtq + 4 * 0x38 + 0x08) = 1.0
set {double}($mtq + 5 * 0x38 + 0x08) = 1.5
call ((void (*)(const double *))SetMomentMTQ)($cmd)
set $i = 0
while $i < 6
  set $out[$i] = {double}($mtq + $i * 0x38)
  set $i = $i + 1
end
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_set_moment_mtq_p1_actual.bin $out ($out + 6)
quit
