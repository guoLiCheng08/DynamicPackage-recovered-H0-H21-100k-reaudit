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
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_pre_y.bin &y ((char *)&y + 264)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_core_input.bin $rbp ($rbp+0x90)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_torque.bin &SatTorque ((char *)&SatTorque + 0x168)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_sat.bin &Sat ((char *)&Sat + 4216)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_globals.bin &J_c_B_mem ((char *)&J_c_B_mem + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_globals.bin &J_c_B_inv ((char *)&J_c_B_inv + 24)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_globals.bin &H_w_B ((char *)&H_w_B + 16)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_globals.bin &L_c_B ((char *)&L_c_B + 16)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_globals.bin &J_c_B ((char *)&J_c_B + 24)
    set $jcb_inv_data = *(void **)((char *)&J_c_B_inv + 16)
    set $hwb_data = *(void **)((char *)&H_w_B + 8)
    set $lcb_data = *(void **)((char *)&L_c_B + 8)
    set $jcb_data = *(void **)((char *)&J_c_B + 16)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_globals.bin $jcb_inv_data ((char *)$jcb_inv_data + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_globals.bin $hwb_data ((char *)$hwb_data + 24)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_globals.bin $lcb_data ((char *)$lcb_data + 24)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_globals.bin $jcb_data ((char *)$jcb_data + 72)
    set $sat_j = *(void **)((char *)&Sat + 0x18)
    set $sat_jinv = *(void **)((char *)&Sat + 0x78)
    set $sat_m3_e8 = *(void **)((char *)&Sat + 0x3e8 + 0x10)
    set $sat_m3_448 = *(void **)((char *)&Sat + 0x448 + 0x10)
    set $sat_m3_4a8 = *(void **)((char *)&Sat + 0x4a8 + 0x10)
    set $sat_m3_508 = *(void **)((char *)&Sat + 0x508 + 0x10)
    set $sat_m3_568 = *(void **)((char *)&Sat + 0x568 + 0x10)
    set $sat_m6 = *(void **)((char *)&Sat + 0x688 + 0x10)
    set $sat_c = *(void **)((char *)&Sat + 0x898 + 0x10)
    set $sat_m9 = *(void **)((char *)&Sat + 0x9a0 + 0x10)
    set $sat_ma = *(void **)((char *)&Sat + 0xa00 + 0x10)
    set $sat_md = *(void **)((char *)&Sat + 0xd38 + 0x10)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_model.bin $sat_j ((char *)$sat_j + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_model.bin $sat_jinv ((char *)$sat_jinv + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_model.bin $sat_m3_e8 ((char *)$sat_m3_e8 + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_model.bin $sat_m3_448 ((char *)$sat_m3_448 + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_model.bin $sat_m3_4a8 ((char *)$sat_m3_4a8 + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_model.bin $sat_m3_568 ((char *)$sat_m3_568 + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_model.bin $sat_m6 ((char *)$sat_m6 + 240)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_model.bin $sat_c ((char *)$sat_c + 240)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_model.bin $sat_m9 ((char *)$sat_m9 + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_model.bin $sat_ma ((char *)$sat_ma + 800)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_model.bin $sat_md ((char *)$sat_md + 800)
  end
  if $core_hits == 2
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_pre_y.bin &y ((char *)&y + 264)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_core_input.bin $rbp ($rbp+0x90)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_torque.bin &SatTorque ((char *)&SatTorque + 0x168)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_sat.bin &Sat ((char *)&Sat + 4216)
    set $sat_j = *(void **)((char *)&Sat + 0x18)
    set $sat_jinv = *(void **)((char *)&Sat + 0x78)
    set $sat_m3_e8 = *(void **)((char *)&Sat + 0x3e8 + 0x10)
    set $sat_m3_448 = *(void **)((char *)&Sat + 0x448 + 0x10)
    set $sat_m3_4a8 = *(void **)((char *)&Sat + 0x4a8 + 0x10)
    set $sat_m3_508 = *(void **)((char *)&Sat + 0x508 + 0x10)
    set $sat_m3_568 = *(void **)((char *)&Sat + 0x568 + 0x10)
    set $sat_m6 = *(void **)((char *)&Sat + 0x688 + 0x10)
    set $sat_c = *(void **)((char *)&Sat + 0x898 + 0x10)
    set $sat_m9 = *(void **)((char *)&Sat + 0x9a0 + 0x10)
    set $sat_ma = *(void **)((char *)&Sat + 0xa00 + 0x10)
    set $sat_md = *(void **)((char *)&Sat + 0xd38 + 0x10)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_model.bin $sat_j ((char *)$sat_j + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_model.bin $sat_jinv ((char *)$sat_jinv + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_model.bin $sat_m3_e8 ((char *)$sat_m3_e8 + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_model.bin $sat_m3_448 ((char *)$sat_m3_448 + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_model.bin $sat_m3_4a8 ((char *)$sat_m3_4a8 + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_model.bin $sat_m3_568 ((char *)$sat_m3_568 + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_model.bin $sat_m6 ((char *)$sat_m6 + 240)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_model.bin $sat_c ((char *)$sat_c + 240)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_model.bin $sat_m9 ((char *)$sat_m9 + 72)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_model.bin $sat_ma ((char *)$sat_ma + 800)
    append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_model.bin $sat_md ((char *)$sat_md + 800)
  end
  continue
end
break *dyn_main+0x7b
commands
  silent
  if $round == 0
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_state.bin $state ($state+33)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_post_y.bin &y ((char *)&y + 264)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_out.bin $out ($out+0x220)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_sts.bin &STS ((char *)&STS + 0x450)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_gyro.bin &Gyro ((char *)&Gyro + 0x640)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_dss.bin &DSS ((char *)&DSS + 0x2b0)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_magmeter.bin &MagMeter ((char *)&MagMeter + 0x270)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_gps.bin &GPS_Kalman ((char *)&GPS_Kalman + 0x70)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_wheels.bin &RWheel ((char *)&RWheel + 0x1e0)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_sada.bin &SADA ((char *)&SADA + 0x68)
    call ((void (*)(void *, void *))sendDynTele)(0,$out)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_ipc.bin $shared ($shared+0xbf0)
    set $round = 1
    set $rdi = $out
    set $rsi = $state
    set $rdx = $cmd
    set $pc = (void *)dyn_main
    continue
  end
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_state.bin $state ($state+33)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_post_y.bin &y ((char *)&y + 264)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_out.bin $out ($out+0x220)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_sts.bin &STS ((char *)&STS + 0x450)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_gyro.bin &Gyro ((char *)&Gyro + 0x640)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_dss.bin &DSS ((char *)&DSS + 0x2b0)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_magmeter.bin &MagMeter ((char *)&MagMeter + 0x270)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_gps.bin &GPS_Kalman ((char *)&GPS_Kalman + 0x70)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_wheels.bin &RWheel ((char *)&RWheel + 0x1e0)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_sada.bin &SADA ((char *)&SADA + 0x68)
  call ((void (*)(void *, void *))sendDynTele)(0,$out)
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_ipc.bin $shared ($shared+0xbf0)
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
  dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_wheelgroup_header.bin &WheelGroup ((char *)&WheelGroup + 0x100)
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
  set {unsigned int}($cmd+0x58) = 1
  set {double}($cmd+0x60) = 0.01
  set {double}($cmd+0x68) = -0.01
  set $shared = (char *)calloc(0xbf0,1)
  call ((int (*)(void *,void *))pthread_rwlock_init)($shared,0)
  set {void **}&g_data = $shared
  set $rdi = $out
  set $rsi = $state
  set $rdx = $cmd
  set $pc = (void *)dyn_main
  continue
end
set $df_calls = 0
set $df2_calls = 0
set $df_abi_calls = 0
set $de_calls = 0
set $de2_calls = 0
break differential_equation
commands
  silent
  if $core_hits == 1
    set $de_calls = $de_calls + 1
    if $de_calls == 1
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_stage1_y.bin $rsi ($rsi+264)
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_stage1_fi_external_desc.bin &F_I_external ((char *)&F_I_external + 16)
      set $fi_external_data = *(void **)((char *)&F_I_external + 8)
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_stage1_fi_external.bin $fi_external_data ((char *)$fi_external_data + 24)
    end
    if $de_calls == 2
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_stage2_y.bin $rsi ($rsi+264)
    end
    if $de_calls == 3
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_stage3_y.bin $rsi ($rsi+264)
    end
    if $de_calls == 4
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_stage4_y.bin $rsi ($rsi+264)
    end
  end
  if $core_hits == 2
    set $de2_calls = $de2_calls + 1
    if $de2_calls == 1
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_stage1_y.bin $rsi ($rsi+264)
    end
    if $de2_calls == 2
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_stage2_y.bin $rsi ($rsi+264)
    end
    if $de2_calls == 3
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_stage3_y.bin $rsi ($rsi+264)
    end
    if $de2_calls == 4
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_stage4_y.bin $rsi ($rsi+264)
    end
  end
  continue
end
break *differential_equation+0x640
commands
  silent
  if $core_hits == 1
    if $de_calls == 1
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_stage1_dydt.bin $rbx ($rbx+264)
    end
    if $de_calls == 2
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_stage2_dydt.bin $rbx ($rbx+264)
    end
    if $de_calls == 3
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_stage3_dydt.bin $rbx ($rbx+264)
    end
    if $de_calls == 4
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_stage4_dydt.bin $rbx ($rbx+264)
    end
  end
  if $core_hits == 2
    if $de2_calls == 1
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_stage1_dydt.bin $rbx ($rbx+264)
    end
    if $de2_calls == 2
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_stage2_dydt.bin $rbx ($rbx+264)
    end
    if $de2_calls == 3
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_stage3_dydt.bin $rbx ($rbx+264)
    end
    if $de2_calls == 4
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_stage4_dydt.bin $rbx ($rbx+264)
    end
  end
  continue
end
break *differential_equation+0x428
commands
  silent
  if $core_hits == 1
    set $df_abi_calls = $df_abi_calls + 1
    if $df_abi_calls == 1
      printf "dynamics_flex ABI rdi=%p rsi=%p rdx=%p rcx=%p r8=%p r9=%p\\n", $rdi, $rsi, $rdx, $rcx, $r8, $r9
      x/2gx $rdi
      x/2gx $rsi
      x/2gx $rdx
      x/2gx $rcx
      x/2gx $r8
      x/2gx $r9
      x/4gx $rsp
      x/3fg *(void **)($rdi+8)
      x/3fg *(void **)($rsi+8)
      x/10fg *(void **)($rdx+8)
      x/10fg *(void **)($rcx+8)
      x/10fg *(void **)($r8+8)
      x/10fg *(void **)($r9+8)
    end
  end
  continue
end
break dynamics_flex
commands
  silent
  if $core_hits == 1
    set $df_calls = $df_calls + 1
    set $df_rigid = $rdi
    set $df_modal_acc = $rdx
    set $df_modal_vel = $r9
  end
  if $core_hits == 2
    set $df2_calls = $df2_calls + 1
    set $df_rigid = $rdi
    set $df_modal_acc = $rdx
    set $df_modal_vel = $r9
  end
  continue
end
break *dynamics_flex+0x3ac
commands
  silent
  if $core_hits == 2 && $df2_calls == 1
    set $m9_input_data = *(void **)($rsi+8)
    set $m9_matrix_data = *(void **)($rdi+16)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_m9_input.bin $m9_input_data ((char *)$m9_input_data+24)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_m9_desc.bin $rdi ((char *)$rdi+24)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_m9_data.bin $m9_matrix_data ((char *)$m9_matrix_data+72)
  end
  continue
end
break *dynamics_flex+0x409
commands
  silent
  if $core_hits == 1 && $df_calls == 1
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_mid_ed29_stack.bin $rsp ($rsp+0x900)
  end
  continue
end
break *dynamics_flex+0x948
commands
  silent
  if $core_hits == 1 && $df_calls == 1
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_pre_inverse_stack.bin $rsp ($rsp+0x900)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_stage1_reaction.bin ($rsp+0x3a0) ($rsp+0x3b8)
  end
  if $core_hits == 1 && $df_calls == 2
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_stage2_reaction.bin ($rsp+0x3a0) ($rsp+0x3b8)
  end
  if $core_hits == 1 && $df_calls == 3
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_stage3_reaction.bin ($rsp+0x3a0) ($rsp+0x3b8)
  end
  if $core_hits == 1 && $df_calls == 4
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_stage4_reaction.bin ($rsp+0x3a0) ($rsp+0x3b8)
  end
  continue
end
break *dynamics_flex+0xb43
commands
  silent
  if $core_hits == 1
    set $df_rigid_data = *(void **)($df_rigid+8)
    set $df_modal_acc_data = *(void **)($df_modal_acc+8)
    set $df_modal_vel_data = *(void **)($df_modal_vel+8)
    if $df_calls == 1
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_stage1.bin $df_rigid_data ($df_rigid_data+24)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_stage1.bin $df_modal_acc_data ($df_modal_acc_data+80)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_stage1.bin $df_modal_vel_data ($df_modal_vel_data+80)
    end
    if $df_calls == 2
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_stage2.bin $df_rigid_data ($df_rigid_data+24)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_stage2.bin $df_modal_acc_data ($df_modal_acc_data+80)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_stage2.bin $df_modal_vel_data ($df_modal_vel_data+80)
    end
    if $df_calls == 3
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_stage3.bin $df_rigid_data ($df_rigid_data+24)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_stage3.bin $df_modal_acc_data ($df_modal_acc_data+80)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_stage3.bin $df_modal_vel_data ($df_modal_vel_data+80)
    end
    if $df_calls == 4
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_stage4.bin $df_rigid_data ($df_rigid_data+24)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_stage4.bin $df_modal_acc_data ($df_modal_acc_data+80)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step1_flex_stage4.bin $df_modal_vel_data ($df_modal_vel_data+80)
    end
  end
  if $core_hits == 2
    set $df_rigid_data = *(void **)($df_rigid+8)
    set $df_modal_acc_data = *(void **)($df_modal_acc+8)
    set $df_modal_vel_data = *(void **)($df_modal_vel+8)
    if $df2_calls == 1
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_flex_stage1.bin $df_rigid_data ($df_rigid_data+24)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_flex_stage1.bin $df_modal_acc_data ($df_modal_acc_data+80)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_flex_stage1.bin $df_modal_vel_data ($df_modal_vel_data+80)
    end
    if $df2_calls == 2
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_flex_stage2.bin $df_rigid_data ($df_rigid_data+24)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_flex_stage2.bin $df_modal_acc_data ($df_modal_acc_data+80)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_flex_stage2.bin $df_modal_vel_data ($df_modal_vel_data+80)
    end
    if $df2_calls == 3
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_flex_stage3.bin $df_rigid_data ($df_rigid_data+24)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_flex_stage3.bin $df_modal_acc_data ($df_modal_acc_data+80)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_flex_stage3.bin $df_modal_vel_data ($df_modal_vel_data+80)
    end
    if $df2_calls == 4
      dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_flex_stage4.bin $df_rigid_data ($df_rigid_data+24)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_flex_stage4.bin $df_modal_acc_data ($df_modal_acc_data+80)
      append binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_sada_position_step2_flex_stage4.bin $df_modal_vel_data ($df_modal_vel_data+80)
    end
  end
  continue
end
break Update_sat_inertia_xw
commands
  silent
  printf "Update_sat_inertia_xw flag=%u core_hits=%u\\n", $edi, $core_hits
  continue
end
run
