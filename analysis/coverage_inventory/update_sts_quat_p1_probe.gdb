set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $sts = $base + 0x217540
set $truth = (char *)calloc(24, 1)
set $truthxyz = (double *)calloc(3, 8)
set $out = (double *)calloc(15, 8)
set {double}$truth = 1.0
set {int}($truth + 8) = 3
set {void **}($truth + 16) = $truthxyz
set $truthxyz[0] = 0.1
set $truthxyz[1] = -0.2
set $truthxyz[2] = 0.3
set $i = 0
while $i < 3
  set $errxyz = (double *)calloc(3, 8)
  set $measxyz = (double *)calloc(3, 8)
  set $errxyz[0] = 0.01 * ($i + 1)
  set $errxyz[1] = -0.02 * ($i + 1)
  set $errxyz[2] = 0.03 * ($i + 1)
  set $item = $sts + $i * 0x170
  set {double}($item + 0x08) = 1.0
  set {double}($item + 0xf8) = 1.0
  set {int}($item + 0x100) = 3
  set {void **}($item + 0x108) = $errxyz
  set {double}($item + 0x130) = 0.0
  set {double}($item + 0x140) = 0.0
  set {int}($item + 0x148) = 3
  set {void **}($item + 0x150) = $measxyz
  set $i = $i + 1
end
call ((void (*)(void *, double))Update_STS_Quat)($truth, 0.25)
set $i = 0
while $i < 3
  set $item = $sts + $i * 0x170
  set $measptr = {double **}($item + 0x150)
  set $out[$i * 5 + 0] = {double}($item + 0x140)
  set $out[$i * 5 + 1] = $measptr[0]
  set $out[$i * 5 + 2] = $measptr[1]
  set $out[$i * 5 + 3] = $measptr[2]
  set $out[$i * 5 + 4] = {double}($item + 0x130)
  set $i = $i + 1
end
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_sts_quat_p1_state.bin $out $out+15
quit
