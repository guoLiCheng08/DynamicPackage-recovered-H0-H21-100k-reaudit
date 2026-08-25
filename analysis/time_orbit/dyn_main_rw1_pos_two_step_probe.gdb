set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set $core_hits = 0
set $round = 0
break CoreDynamic
commands
  silent
  set $core_hits = $core_hits + 1
  continue
end
break *CoreDynamic+0x4a6
commands
  silent
  if $core_hits == 1
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step1_pre_y.bin &y ((char *)&y + 264)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step1_core_input.bin $rbp ($rbp+0x90)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step1_torque.bin &SatTorque ((char *)&SatTorque + 0x168)
  end
  if $core_hits == 2
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step2_pre_y.bin &y ((char *)&y + 264)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step2_core_input.bin $rbp ($rbp+0x90)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step2_torque.bin &SatTorque ((char *)&SatTorque + 0x168)
  end
  continue
end
break *dyn_main+0x7b
commands
  silent
  if $round == 0
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step1_state.bin $state ($state+33)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step1_post_y.bin &y ((char *)&y + 264)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step1_out.bin $out ($out+0x220)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step1_sts.bin &STS ((char *)&STS + 0x450)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step1_gyro.bin &Gyro ((char *)&Gyro + 0x640)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step1_dss.bin &DSS ((char *)&DSS + 0x2b0)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step1_magmeter.bin &MagMeter ((char *)&MagMeter + 0x270)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step1_gps.bin &GPS_Kalman ((char *)&GPS_Kalman + 0x70)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step1_wheels.bin &RWheel ((char *)&RWheel + 0x1e0)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step1_sada.bin &SADA ((char *)&SADA + 0x68)
    call ((void (*)(void *, void *))sendDynTele)(0,$out)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step1_ipc.bin $shared ($shared+0xbf0)
    set $round = 1
    set $rdi = $out
    set $rsi = $state
    set $rdx = $cmd
    set $pc = (void *)dyn_main
    continue
  end
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step2_state.bin $state ($state+33)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step2_post_y.bin &y ((char *)&y + 264)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step2_out.bin $out ($out+0x220)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step2_sts.bin &STS ((char *)&STS + 0x450)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step2_gyro.bin &Gyro ((char *)&Gyro + 0x640)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step2_dss.bin &DSS ((char *)&DSS + 0x2b0)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step2_magmeter.bin &MagMeter ((char *)&MagMeter + 0x270)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step2_gps.bin &GPS_Kalman ((char *)&GPS_Kalman + 0x70)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step2_wheels.bin &RWheel ((char *)&RWheel + 0x1e0)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step2_sada.bin &SADA ((char *)&SADA + 0x68)
  call ((void (*)(void *, void *))sendDynTele)(0,$out)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_step2_ipc.bin $shared ($shared+0xbf0)
  kill
  quit
end
break main
commands
  silent
  set $init = (char *)calloc(0xe8,1)
  set {double}($init+0x00) = 0.1
  set {double}($init+0x08) = 1.0
  set {float}($init+0x28) = 0.001
  set {float}($init+0x2c) = -0.002
  set {float}($init+0x30) = 0.003
  set {double}($init+0x38) = 2025.0
  set {double}($init+0x40) = 1.0
  set {double}($init+0x48) = 2.0
  set {double}($init+0x50) = 3.0
  set {double}($init+0x58) = 4.0
  set {double}($init+0x60) = 5.0
  set {double}($init+0x68) = 7000000.0
  set {double}($init+0x70) = 0.01
  set {double}($init+0x78) = 0.5
  set {double}($init+0x80) = 0.2
  set {double}($init+0x88) = 0.3
  set {double}($init+0x90) = 0.4
  set {double}($init+0x98) = 120.0
  set {double}($init+0xc0) = 100.0
  set {double}($init+0xd8) = 80.0
  set {double}($init+0xe0) = 1000.0
  call ((void (*)(void *))dyn_init)($init)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_rw1_pos_wheelgroup_header.bin &WheelGroup ((char *)&WheelGroup + 0x100)
  call ((void (*)(unsigned int))srand)(1)
  set $state = (double *)calloc(33,8)
  set $state[0] = 1.0
  set $state[4] = 0.001
  set $state[5] = -0.002
  set $state[6] = 0.003
  set $state[7] = 7000000.0
  set $state[11] = 7546.0
  set $out = (char *)calloc(0x220,1)
  set $cmd = (char *)calloc(0x78,1)
  set {double}($cmd+0x08) = 0.001
  set $shared = (char *)calloc(0xbf0,1)
  call ((int (*)(void *,void *))pthread_rwlock_init)($shared,0)
  set {void **}&g_data = $shared
  set $rdi = $out
  set $rsi = $state
  set $rdx = $cmd
  set $pc = (void *)dyn_main
  continue
end
run
