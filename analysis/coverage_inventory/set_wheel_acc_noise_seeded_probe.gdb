set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
call srand(1)
set $base = (char *)&main - 0x1340
set $wheels = $base + 0x217e00
set $cmd = (double *)calloc(4, 8)
set $out = (double *)calloc(4, 8)
set $i = 0
while $i < 4
  set $wheel = $wheels + $i * 0x78
  set {int}($wheel + 4) = 0
  set {double}($wheel + 0x18) = 100.0
  set {double}($wheel + 0x20) = 10.0
  set {double}($wheel + 0x28) = 2.0
  set {double}($wheel + 0x30) = 0.0
  set {double}($wheel + 0x38) = -99.0
  set $i = $i + 1
end
# 仅 2 号轮走噪声：candidate=4/2=2，sigma=0.5/2=0.25；扭矩与速度均不触发限幅。
set $cmd[2] = 4.0
set $wheel2 = $wheels + 2 * 0x78
set {int}($wheel2 + 4) = 1
set {double}($wheel2 + 0x10) = 0.5
call ((void (*)(const double *))SetWheelAcc)($cmd)
set $i = 0
while $i < 4
  set $wheel = $wheels + $i * 0x78
  set $out[$i] = {double}($wheel + 0x38)
  set $i = $i + 1
end
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_set_wheel_acc_noise_seeded_p1_accel.bin $out ($out + 4)
quit
