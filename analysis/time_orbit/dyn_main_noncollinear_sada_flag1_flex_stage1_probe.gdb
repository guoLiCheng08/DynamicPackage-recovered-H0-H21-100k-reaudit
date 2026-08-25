set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set $core_hits = 0
set $round = 0
set $mag_torque_captured = 0
set $flex_hits = 0
set $flex_return_hits = 0
break *differential_equation+0x42d
commands
  silent
  set $flex_return_hits = $flex_return_hits + 1
  if $core_hits == 1 && $flex_return_hits == 1
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_rigid_out.bin ($rsp+0x30) ($rsp+0x48)
    set $rigid_out_data = *(void **)($rsp+0x38)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_rigid_out_data.bin $rigid_out_data ((char *)$rigid_out_data+24)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_postcall_e0.bin ($rsp+0xe0) ($rsp+0x100)
  end
  if $core_hits == 2 && $flex_return_hits == 5
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_stage1_flex_rigid_out.bin ($rsp+0x30) ($rsp+0x48)
    set $step2_rigid_out_data = *(void **)($rsp+0x38)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_stage1_flex_rigid_out_data.bin $step2_rigid_out_data ((char *)$step2_rigid_out_data+24)
  end
  continue
end
break *dynamics_flex+0x4fc
commands
  silent
  if $core_hits == 2 && $flex_hits == 5
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_stage1_h_total.bin ($rsp+0x340) ($rsp+0x358)
  end
  continue
end
break dynamics_flex
commands
  silent
  set $flex_hits = $flex_hits + 1
  if $core_hits == 1 && $flex_hits == 1
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_stack.bin ($rsp-0x40) ($rsp+0x500)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_rdi.bin $rdi ($rdi+24)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_rsi.bin $rsi ($rsi+24)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_rdx.bin $rdx ($rdx+24)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_rcx.bin $rcx ($rcx+24)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_r8.bin $r8 ($r8+24)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_r9.bin $r9 ($r9+24)
    set $rsi_data = *(void **)($rsi+8)
    set $rsi_next_data = *(void **)($rsi+24)
    set $rdx_data = *(void **)($rdx+8)
    set $rcx_data = *(void **)($rcx+8)
    set $r8_data = *(void **)($r8+8)
    set $r9_data = *(void **)($r9+8)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_rsi_data.bin $rsi_data ((char *)$rsi_data+24)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_rsi_next_data.bin $rsi_next_data ((char *)$rsi_next_data+24)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_modal_accel.bin $rdx_data ((char *)$rdx_data+80)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dynamics_flex_gold.bin $rdx_data ((char *)$rdx_data+80)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_rcx_data.bin $rcx_data ((char *)$rcx_data+80)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_r8_data.bin $r8_data ((char *)$r8_data+80)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_stage1_flex_r9_data.bin $r9_data ((char *)$r9_data+80)
  end
  continue
end
break MagTorque
commands
  silent
  if $round == 1 && $mag_torque_captured == 0
    set $mag_moment_data = *(void **)($rsi+0x8)
    set $mag_body_data = *(void **)($rdx+0x8)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_magtorque_moment.bin $mag_moment_data ((char *)$mag_moment_data+24)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_magtorque_body.bin $mag_body_data ((char *)$mag_body_data+24)
    set $mag_torque_captured = 1
  end
  continue
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
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_pre_y.bin &y ((char *)&y + 264)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_core_input.bin $rbp ($rbp+0x90)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_torque.bin &SatTorque ((char *)&SatTorque + 0x168)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_bi_static.bin &B_I_static ((char *)&B_I_static + 0x30)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_mtq_group.bin &MTQ_Group ((char *)&MTQ_Group + 0x140)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_j.bin &J_c_B_mem ((char *)&J_c_B_mem + 72)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_jinv.bin &J_c_B_inv_mem ((char *)&J_c_B_inv_mem + 72)
    set $m4a8 = *(void **)((char *)&Sat + 0x4a8 + 0x10)
    set $m6 = *(void **)((char *)&Sat + 0x688 + 0x10)
    set $c = *(void **)((char *)&Sat + 0x898 + 0x10)
    set $m9 = *(void **)((char *)&Sat + 0x9a0 + 0x10)
    set $ma = *(void **)((char *)&Sat + 0xa00 + 0x10)
    set $md = *(void **)((char *)&Sat + 0xd38 + 0x10)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_used_model.bin $m4a8 ((char *)$m4a8 + 72)
    append binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_used_model.bin $m6 ((char *)$m6 + 240)
    append binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_used_model.bin $c ((char *)$c + 240)
    append binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_used_model.bin $m9 ((char *)$m9 + 72)
    append binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_used_model.bin $ma ((char *)$ma + 800)
    append binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_used_model.bin $md ((char *)$md + 800)
  end
  if $core_hits == 2
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_pre_y.bin &y ((char *)&y + 264)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_core_input.bin $rbp ($rbp+0x90)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_torque.bin &SatTorque ((char *)&SatTorque + 0x168)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_bi_static.bin &B_I_static ((char *)&B_I_static + 0x30)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_mtq_group.bin &MTQ_Group ((char *)&MTQ_Group + 0x140)
    set $bi_data = *(void **)((char *)&B_I_static + 0x8)
    set $mtq_group_data = *(void **)((char *)&MTQ_Group + 0x8)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_bi_vector.bin $bi_data ((char *)$bi_data + 24)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_mtq_group_vector.bin $mtq_group_data ((char *)$mtq_group_data + 24)
    set $sat_attitude_data = (char *)&Sat + 0x1a8
    set $sat_attitude_matrix = *(void **)($sat_attitude_data + 0x10)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_sat_attitude.bin $sat_attitude_data ($sat_attitude_data + 24)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_sat_attitude_matrix.bin $sat_attitude_matrix ((char *)$sat_attitude_matrix + 72)
  end
  continue
end
break *dyn_main+0x7b
commands
  silent
  if $round == 0
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_state.bin $state ($state+33)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_post_y.bin &y ((char *)&y + 264)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_out.bin $out ($out+0x220)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_sts.bin &STS ((char *)&STS + 0x450)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_gyro.bin &Gyro ((char *)&Gyro + 0x640)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_dss.bin &DSS ((char *)&DSS + 0x2b0)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_magmeter.bin &MagMeter ((char *)&MagMeter + 0x270)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_gps.bin &GPS_Kalman ((char *)&GPS_Kalman + 0x70)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_wheels.bin &RWheel ((char *)&RWheel + 0x1e0)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_sada.bin &SADA ((char *)&SADA + 0x68)
    call ((void (*)(void *, void *))sendDynTele)(0,$out)
    dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step1_ipc.bin $shared ($shared+0xbf0)
    set $round = 1
    set $rdi = $out
    set $rsi = $state
    set $rdx = $cmd
    set $pc = (void *)dyn_main
    continue
  end
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_state.bin $state ($state+33)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_post_y.bin &y ((char *)&y + 264)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_out.bin $out ($out+0x220)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_sts.bin &STS ((char *)&STS + 0x450)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_gyro.bin &Gyro ((char *)&Gyro + 0x640)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_dss.bin &DSS ((char *)&DSS + 0x2b0)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_magmeter.bin &MagMeter ((char *)&MagMeter + 0x270)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_gps.bin &GPS_Kalman ((char *)&GPS_Kalman + 0x70)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_wheels.bin &RWheel ((char *)&RWheel + 0x1e0)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_sada.bin &SADA ((char *)&SADA + 0x68)
  call ((void (*)(void *, void *))sendDynTele)(0,$out)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_step2_ipc.bin $shared ($shared+0xbf0)
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
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_noncollinear_sada_flag1_wheelgroup_header.bin &WheelGroup ((char *)&WheelGroup + 0x100)
  call ((void (*)(unsigned int))srand)(1)
  set $state = (double *)calloc(33,8)
  set $state[0] = 0.5
  set $state[1] = 0.5
  set $state[2] = -0.5
  set $state[3] = 0.5
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
  set {unsigned int}($cmd+0x74) = 1
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
