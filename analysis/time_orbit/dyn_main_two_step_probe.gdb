set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/dyn_main_two_step_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $init = (char *)calloc(0xe8,1)
  # +0x00 step; +0x08 scalar attitude component; +0x10..+0x20 vector components
  set {double}($init+0x00) = 0.1
  set {double}($init+0x08) = 1.0
  # +0x28..+0x30: initial body rate float
  set {float}($init+0x28) = 0.001
  set {float}($init+0x2c) = -0.002
  set {float}($init+0x30) = 0.003
  # +0x38..+0x60 TimeInit arguments
  set {double}($init+0x38) = 2025.0
  set {double}($init+0x40) = 1.0
  set {double}($init+0x48) = 2.0
  set {double}($init+0x50) = 3.0
  set {double}($init+0x58) = 4.0
  set {double}($init+0x60) = 5.0
  # +0x68..+0x90 orbital elements a,e,i,Omega,omega,M
  set {double}($init+0x68) = 7000000.0
  set {double}($init+0x70) = 0.01
  set {double}($init+0x78) = 0.5
  set {double}($init+0x80) = 0.2
  set {double}($init+0x88) = 0.3
  set {double}($init+0x90) = 0.4
  # +0x98 inertia matrix identity; +0xe0 spacecraft mass
  set {double}($init+0x98) = 120.0
  set {double}($init+0xc0) = 100.0
  set {double}($init+0xd8) = 80.0
  set {double}($init+0xe0) = 1000.0
  call ((void (*)(void *))dyn_init)($init)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_init_sat.bin &Sat ((char *)&Sat + 0x1078)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_init_sat_torque.bin &SatTorque ((char *)&SatTorque + 0x168)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_init_wheel_group.bin &WheelGroup ((char *)&WheelGroup + 0xc8)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_init_mtq_group.bin &MTQ_Group ((char *)&MTQ_Group + 0x110)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_init_thruster.bin &Thruster ((char *)&Thruster + 0xb0)
  call ((void (*)(unsigned int))srand)(1)
  set $state = (double *)calloc(33,8)
  set $state[0] = 1.0
  set $state[4] = 0.001
  set $state[5] = -0.002
  set $state[6] = 0.003
  set $state[7] = 7000000.0
  set $state[8] = 0.0
  set $state[9] = 0.0
  set $state[10] = 0.0
  set $state[11] = 7546.0
  set $state[12] = 0.0
  set $out = (char *)calloc(0x220,1)
  set $cmd = (char *)calloc(0x78,1)
  set $shared = (char *)calloc(0xbf0,1)
  call ((int (*)(void *, void *))pthread_rwlock_init)($shared,0)
  set {void **}&g_data = $shared
  call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_main_step1_state.bin $state ($state+33)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_main_step1_out.bin $out ($out+0x220)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_main_step1_sts.bin &STS ((char *)&STS + 0x450)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_main_step1_gyro.bin &Gyro ((char *)&Gyro + 0x640)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_main_step1_dss.bin &DSS ((char *)&DSS + 0x2b0)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_main_step1_magmeter.bin &MagMeter ((char *)&MagMeter + 0x270)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_main_step1_gps.bin &GPS_Kalman ((char *)&GPS_Kalman + 0x70)
  call ((void (*)(void *, void *))sendDynTele)(0,$out)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_main_step1_ipc.bin $shared ($shared+0xbf0)
  call ((void (*)(void *,void *,void *))dyn_main)($out,$state,$cmd)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_main_step2_state.bin $state ($state+33)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_main_step2_out.bin $out ($out+0x220)
  call ((void (*)(void *, void *))sendDynTele)(0,$out)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_dyn_main_step2_ipc.bin $shared ($shared+0xbf0)
  printf "dyn_main_step2_state\n"
  x/33gx $state
  printf "dyn_main_step2_out_prefix\n"
  x/16gx $out
  kill
  quit
end
run
