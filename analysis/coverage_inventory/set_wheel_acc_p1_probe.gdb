set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $wheels = $base + 0x217e00
set $cmd = (double *)calloc(4, 8)
set $out = (double *)calloc(4, 8)
# 全轮：无噪声、惯量=2、转矩极限=10、速度极限=100。
set $i = 0
while $i < 4
  set $wheel = $wheels + $i * 0x78
  set {int}($wheel + 4) = 0
  set {double}($wheel + 0x18) = 100.0
  set {double}($wheel + 0x20) = 10.0
  set {double}($wheel + 0x28) = 2.0
  set {double}($wheel + 0x38) = -99.0
  set $i = $i + 1
end
# 0：12/2=6，经 10/2=5 限幅；1：精确 +omega_limit，旧 acceleration 应保留；
# 2：正向超限再正向命令，写 0；3：负向超限再负向命令，写 0。
set $cmd[0] = 12.0
set $cmd[1] = 4.0
set $cmd[2] = 4.0
set $cmd[3] = -4.0
set {double}($wheels + 0x00 * 0x78 + 0x30) = 0.0
set {double}($wheels + 0x01 * 0x78 + 0x30) = 100.0
set {double}($wheels + 0x02 * 0x78 + 0x30) = 120.0
set {double}($wheels + 0x03 * 0x78 + 0x30) = -120.0
call ((void (*)(const double *))SetWheelAcc)($cmd)
set $i = 0
while $i < 4
  set $wheel = $wheels + $i * 0x78
  set $out[$i] = {double}($wheel + 0x38)
  set $i = $i + 1
end
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_set_wheel_acc_p1_accel.bin $out ($out + 4)
quit
