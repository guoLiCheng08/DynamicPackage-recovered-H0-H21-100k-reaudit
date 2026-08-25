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
  set {float}($init+0x28)=0.001
  set {float}($init+0x2c)=-0.002
  set {float}($init+0x30)=0.003
  set {double}($init+0x38)=2025.0
  set {double}($init+0x40)=1.0
  set {double}($init+0x48)=2.0
  set {double}($init+0x50)=3.0
  set {double}($init+0x58)=4.0
  set {double}($init+0x60)=5.0
  set {double}($init+0x68)=7000000.0
  set {double}($init+0x70)=0.01
  set {double}($init+0x78)=0.5
  set {double}($init+0x80)=0.2
  set {double}($init+0x88)=0.3
  set {double}($init+0x90)=0.4
  set {double}($init+0x98)=120.0
  set {double}($init+0xc0)=100.0
  set {double}($init+0xd8)=80.0
  set {double}($init+0xe0)=1000.0
  call ((void (*)(void *))dyn_init)($init)
  call ((void (*)(unsigned int))srand)(1)
  set {double}((char *)&RWheel+0x30)=628.4185307179587
  set {double}((char *)&RWheel+0x38)=0.0
  set {double}((char *)&RWheel+0x40)=7.981915339118876
  set $state=(double *)calloc(33,8)
  set $state[0]=0.5
  set $state[1]=0.5
  set $state[2]=-0.5
  set $state[3]=0.5
  set $state[4]=0.001
  set $state[5]=-0.002
  set $state[6]=0.003
  set $state[7]=7000000.0
  set $state[11]=7546.0
  set $out=(char *)calloc(0x220,1)
  set $cmd=(char *)calloc(0x78,1)
  set $shared=(char *)calloc(0xbf0,1)
  call ((int (*)(void *,void *))pthread_rwlock_init)($shared,0)
  set {void *}0x5555556184b0=$shared
  set $payload=$shared+0x38
  set {double}($cmd+0x08)=0.01
  call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_wheel_limit_step1_state.bin $state ($state+33)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_wheel_limit_step1_out.bin $out ($out+0x220)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_wheel_limit_step1_rwheel.bin &RWheel ((char *)&RWheel+0x1e0)
  call ((void (*)(void *,void *))sendDynTele)(0,$out)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_wheel_limit_step1_ipc_payload.bin $payload ($shared+0xbf0)
  call ((void *(*)(void *,int,unsigned long))memset)($cmd,0,0x78)
  set {double}($cmd+0x08)=-0.01
  call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_wheel_limit_step2_state.bin $state ($state+33)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_wheel_limit_step2_out.bin $out ($out+0x220)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_wheel_limit_step2_rwheel.bin &RWheel ((char *)&RWheel+0x1e0)
  call ((void (*)(void *,void *))sendDynTele)(0,$out)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_wheel_limit_step2_ipc_payload.bin $payload ($shared+0xbf0)
  kill
  quit
end
run
