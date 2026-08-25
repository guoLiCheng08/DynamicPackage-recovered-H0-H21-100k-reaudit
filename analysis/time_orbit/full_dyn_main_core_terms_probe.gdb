set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set $core_hits = 0
set $dyn_round = 0
set $gaussian_step1_calls = 0
set $gaussian_step2_calls = 0
set $rand_step1_calls = 0
set $rand_step2_calls = 0
set $rand_before_measure_step1 = 0
set $rand_before_measure_step2 = 0
set $quat_cross_calls = 0
break UpdateDeviceMeasure
commands
  silent
  if $dyn_round == 0
    set $rand_before_measure_step1 = $rand_step1_calls
  else
    set $rand_before_measure_step2 = $rand_step2_calls
  end
  continue
end
break rand
commands
  silent
  if $dyn_round == 0
    set $rand_step1_calls = $rand_step1_calls + 1
  else
    set $rand_step2_calls = $rand_step2_calls + 1
  end
  continue
end
break quat_cross
commands
  silent
  set $quat_cross_calls = $quat_cross_calls + 1
  continue
end
break *quat_cross+0x85
commands
  silent
  if $quat_cross_calls == 7
    set $out_xyz = *(void **)($rbx+0x10)
    set $rsi_xyz = *(void **)($r13+0x10)
    set $rdx_xyz = *(void **)($r12+0x10)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_sts0_quatcross_out_abi.bin $rbx ($rbx+0x18)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_sts0_quatcross_out_xyz.bin $out_xyz ((char *)$out_xyz+24)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_sts0_quatcross_rsi_abi.bin $r13 ($r13+0x18)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_sts0_quatcross_rsi_xyz.bin $rsi_xyz ((char *)$rsi_xyz+24)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_sts0_quatcross_rdx_abi.bin $r12 ($r12+0x18)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_sts0_quatcross_rdx_xyz.bin $rdx_xyz ((char *)$rdx_xyz+24)
  end
  continue
end
break ran_gaussian2
commands
  silent
  if $dyn_round == 0
    set $gaussian_step1_calls = $gaussian_step1_calls + 1
  else
    set $gaussian_step2_calls = $gaussian_step2_calls + 1
  end
  continue
end
break *dyn_main+0x7b
commands
  silent
  if $dyn_round == 0
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_post_y.bin &y ((char *)&y + 264)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_sts.bin &STS ((char *)&STS + 0x450)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_gyro.bin &Gyro ((char *)&Gyro + 0x640)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_dss.bin &DSS ((char *)&DSS + 0x2b0)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_magmeter.bin &MagMeter ((char *)&MagMeter + 0x270)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_gps.bin &GPS_Kalman ((char *)&GPS_Kalman + 0x70)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_wheels.bin &RWheel ((char *)&RWheel + 0x1e0)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_sada.bin &SADA ((char *)&SADA + 0x68)
    set $dyn_round = 1
    set $rdi = $out
    set $rsi = $state
    set $rdx = $cmd
    set $pc = (void *)dyn_main
    continue
  end
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_post_y.bin &y ((char *)&y + 264)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_sts.bin &STS ((char *)&STS + 0x450)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_gyro.bin &Gyro ((char *)&Gyro + 0x640)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_dss.bin &DSS ((char *)&DSS + 0x2b0)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_magmeter.bin &MagMeter ((char *)&MagMeter + 0x270)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_gps.bin &GPS_Kalman ((char *)&GPS_Kalman + 0x70)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_wheels.bin &RWheel ((char *)&RWheel + 0x1e0)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_sada.bin &SADA ((char *)&SADA + 0x68)
  printf "ran_gaussian2 calls: step1=%d step2=%d; rand calls: step1=%d step2=%d; before_measure: step1=%d step2=%d\\n", $gaussian_step1_calls, $gaussian_step2_calls, $rand_step1_calls, $rand_step2_calls, $rand_before_measure_step1, $rand_before_measure_step2
  kill
  quit
end
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
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_pre_y.bin &y ((char *)&y + 264)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_core_input.bin $rbp ($rbp+0x90)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_torque.bin &SatTorque ((char *)&SatTorque + 0x168)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_sat.bin &Sat ((char *)&Sat + 4216)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_flex_globals.bin &J_c_B_mem ((char *)&J_c_B_mem + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_flex_globals.bin &J_c_B_inv ((char *)&J_c_B_inv + 24)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_flex_globals.bin &H_w_B ((char *)&H_w_B + 16)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_flex_globals.bin &L_c_B ((char *)&L_c_B + 16)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_flex_globals.bin &J_c_B ((char *)&J_c_B + 24)
    set $jcb_inv_data = *(void **)((char *)&J_c_B_inv + 16)
    set $hwb_data = *(void **)((char *)&H_w_B + 8)
    set $lcb_data = *(void **)((char *)&L_c_B + 8)
    set $jcb_data = *(void **)((char *)&J_c_B + 16)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_flex_globals.bin $jcb_inv_data ((char *)$jcb_inv_data + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_flex_globals.bin $hwb_data ((char *)$hwb_data + 24)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_flex_globals.bin $lcb_data ((char *)$lcb_data + 24)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step1_flex_globals.bin $jcb_data ((char *)$jcb_data + 72)
  end
  if $core_hits == 2
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_pre_y.bin &y ((char *)&y + 264)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_core_input.bin $rbp ($rbp+0x90)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_step2_torque.bin &SatTorque ((char *)&SatTorque + 0x168)
  end
  continue
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
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_init_y.bin &y ((char *)&y + 264)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_init_wheels.bin &RWheel ((char *)&RWheel + 0x1e0)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_init_mtq.bin &MTQ ((char *)&MTQ + 0x150)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_init_sada.bin &SADA ((char *)&SADA + 0x68)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_init_sts.bin &STS ((char *)&STS + 0x450)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_init_gyro.bin &Gyro ((char *)&Gyro + 0x640)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_init_dss.bin &DSS ((char *)&DSS + 0x2b0)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_full_dyn_init_magmeter.bin &MagMeter ((char *)&MagMeter + 0x270)
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
  set $rdi = $out
  set $rsi = $state
  set $rdx = $cmd
  set $pc = (void *)dyn_main
  continue
end
run
