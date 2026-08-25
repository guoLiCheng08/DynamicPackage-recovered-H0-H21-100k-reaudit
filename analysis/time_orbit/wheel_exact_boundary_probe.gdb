set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $init=(char *)calloc(0xe8,1)
  set {double}($init+0x00)=0.1
  set {double}($init+0x08)=1.0
  set {double}($init+0x68)=7000000.0
  set {double}($init+0x98)=120.0
  set {double}($init+0xc0)=100.0
  set {double}($init+0xd8)=80.0
  set {double}($init+0xe0)=1000.0
  call ((void (*)(void *))dyn_init)($init)
  set $w=(char *)&RWheel
  set $cmd=(double *)calloc(4,8)
  set {unsigned int}($w+0x04)=0
  set {double}($w+0x18)=1.0
  set {double}($w+0x20)=0.05
  set {double}($w+0x28)=0.1
  set {double}($w+0x30)=1.0
  set {double}($w+0x38)=0.25
  set $cmd[0]=0.05
  call ((void (*)(void *))SetWheelAcc)($cmd)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_wheel_exact_limit_pos.bin $w ($w+0x78)
  set {double}($w+0x30)=-2.0
  set {double}($w+0x38)=0.0
  set $cmd[0]=-1.0
  call ((void (*)(void *))SetWheelAcc)($cmd)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_wheel_negative_high_torque_limit.bin $w ($w+0x78)
  set {double}($w+0x30)=2.0
  set {double}($w+0x38)=0.0
  set $cmd[0]=1.0
  call ((void (*)(void *))SetWheelAcc)($cmd)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_wheel_positive_high_torque_limit.bin $w ($w+0x78)
  kill
  quit
end
run
