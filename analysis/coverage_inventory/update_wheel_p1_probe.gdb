set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $wheels = $base + 0x217e00
set $group = $base + 0x217d20
set $h = (double *)calloc(3, 8)
set $torque3 = (double *)calloc(3, 8)
set $map = (double *)calloc(12, 8)
set $derived = (double *)calloc(12, 8)
# RWheel: inertia, omega, acceleration; output fields torque/omega/H are captured after one dt=0.25 step.
set {double}($wheels + 0 * 0x78 + 0x28) = 2.0
set {double}($wheels + 0 * 0x78 + 0x30) = 10.0
set {double}($wheels + 0 * 0x78 + 0x38) = 1.0
set {double}($wheels + 1 * 0x78 + 0x28) = 3.0
set {double}($wheels + 1 * 0x78 + 0x30) = -20.0
set {double}($wheels + 1 * 0x78 + 0x38) = -2.0
set {double}($wheels + 2 * 0x78 + 0x28) = 4.0
set {double}($wheels + 2 * 0x78 + 0x30) = 30.0
set {double}($wheels + 2 * 0x78 + 0x38) = 0.5
set {double}($wheels + 3 * 0x78 + 0x28) = 5.0
set {double}($wheels + 3 * 0x78 + 0x30) = -40.0
set {double}($wheels + 3 * 0x78 + 0x38) = -1.5
set $h[0] = 100.0
set $h[1] = 200.0
set $h[2] = 300.0
# 3x4 row-major non-symmetric mapping.
set $map[0] = 1.0
set $map[1] = -2.0
set $map[2] = 0.5
set $map[3] = 3.0
set $map[4] = -0.25
set $map[5] = 1.5
set $map[6] = 2.0
set $map[7] = -3.0
set $map[8] = 4.0
set $map[9] = -1.5
set $map[10] = 0.25
set $map[11] = 0.5
# WheelGroup vector/matrix descriptors.
set {int}($group + 0x00) = 3
set {void **}($group + 0x08) = $h
set {int}($group + 0x28) = 3
set {void **}($group + 0x30) = $torque3
set {int}($group + 0x50) = 3
set {int}($group + 0x54) = 4
set {int}($group + 0x58) = 4
set {void **}($group + 0x60) = $map
call ((void (*)(double))UpdateWheel)(0.25)
set $i = 0
while $i < 4
  set $wheel = $wheels + $i * 0x78
  set $derived[$i * 3 + 0] = {double}($wheel + 0x48)
  set $derived[$i * 3 + 1] = {double}($wheel + 0x30)
  set $derived[$i * 3 + 2] = {double}($wheel + 0x40)
  set $i = $i + 1
end
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_wheel_p1_derived.bin $derived ($derived + 12)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_wheel_p1_group_torque.bin $torque3 ($torque3 + 3)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_wheel_p1_group_h.bin $h ($h + 3)
quit
