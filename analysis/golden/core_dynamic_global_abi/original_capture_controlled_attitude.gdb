set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/original_capture.log
set logging overwrite on
set logging enabled on
break *CoreDynamic
commands
  silent
  set $out_entry = $rdi
  set $input_entry = $rsi
  set ((double *)&y)[0] = 0.86602540378443864676
  set ((double *)&y)[1] = 0.5
  set ((double *)&y)[2] = 0.0
  set ((double *)&y)[3] = 0.0
  set $y_begin = (char *)&y
  set $y_end = $y_begin + 264
  set $t_begin = (char *)&t
  set $t_end = $t_begin + 8
  set $hdata = *(void **)((char *)&H_w_B+8)
  set $ldata = *(void **)((char *)&L_c_B+8)
  set $jdata = *(void **)((char *)&J_c_B+16)
  set $jidata = *(void **)((char *)&J_c_B_inv+16)
  set $fdata = *(void **)((char *)&F_I_external+8)
  set $m4a8_data = *(void **)((char *)&Sat+0x4b8)
  set $m688_data = *(void **)((char *)&Sat+0x698)
  set $coupling_data = *(void **)((char *)&Sat+0x8a8)
  set $m9_data = *(void **)((char *)&Sat+0x9b0)
  set $ma_data = *(void **)((char *)&Sat+0xa10)
  set $md_data = *(void **)((char *)&Sat+0xd48)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/pre_input_90.bin $input_entry ($input_entry+0x90)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/pre_output_148.bin $out_entry ($out_entry+0x148)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/pre_y_33.bin $y_begin $y_end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/pre_t.bin $t_begin $t_end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/step_time.bin &step_time ((char *)&step_time+8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/pre_sat_1078.bin &Sat ((char *)&Sat+0x1078)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/pre_sat_torque_168.bin &SatTorque ((char *)&SatTorque+0x168)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/j_c_b_data.bin $jdata ((char *)$jdata+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/j_c_b_inv_data.bin $jidata ((char *)$jidata+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/h_w_b_data.bin $hdata ((char *)$hdata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/l_c_b_data.bin $ldata ((char *)$ldata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/f_i_external_data.bin $fdata ((char *)$fdata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/spacecraft_mass.bin &SpacecraftMass ((char *)&SpacecraftMass+8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/sada_raw.bin &SADA ((char *)&SADA+104)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/sat_m4a8_data.bin $m4a8_data ((char *)$m4a8_data+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/sat_m688_data.bin $m688_data ((char *)$m688_data+240)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/sat_coupling_data.bin $coupling_data ((char *)$coupling_data+240)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/sat_m9_data.bin $m9_data ((char *)$m9_data+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/sat_modal_a_data.bin $ma_data ((char *)$ma_data+800)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/sat_modal_d_data.bin $md_data ((char *)$md_data+800)
  printf "core_entry out=%p input=%p y0=%.17g\n", $out_entry, $input_entry, ((double *)&y)[0]
  continue
end
break *CoreDynamic+0x700
commands
  silent
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/post_output_148.bin $out_entry ($out_entry+0x148)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/post_y_33.bin $y_begin $y_end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/post_t.bin $t_begin $t_end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/post_sat_1078.bin &Sat ((char *)&Sat+0x1078)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/post_sat_torque_168.bin &SatTorque ((char *)&SatTorque+0x168)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/post_h_w_b_data.bin $hdata ((char *)$hdata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/post_l_c_b_data.bin $ldata ((char *)$ldata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_attitude/post_f_i_external_data.bin $fdata ((char *)$fdata+24)
  printf "core_exit t=%.17g y0=%.17g out0=%.17g\n", *(double *)&t, ((double *)&y)[0], *(double *)$out_entry
  kill
  quit
end
break main
commands
  silent
  set $init = (char *)calloc(0xe8, 1)
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
  set $state = (double *)calloc(33, 8)
  set $state[0] = 0.86602540378443864676
  set $state[2] = 0.5
  set $state[4] = 0.001
  set $state[5] = -0.002
  set $state[6] = 0.003
  set $state[7] = 7000000.0
  set $state[11] = 7546.0
  set $cmd = (char *)calloc(0x78, 1)
  set {unsigned int}($cmd+0x00) = 1
  set {double}($cmd+0x08) = 0.010
  set {double}($cmd+0x10) = -0.005
  set {double}($cmd+0x18) = 0.002
  set {double}($cmd+0x20) = 0.000
  set {double}($cmd+0x28) = 0.001
  set {double}($cmd+0x30) = -0.002
  set {double}($cmd+0x38) = 0.003
  set {unsigned int}($cmd+0x58) = 1
  set {double}($cmd+0x60) = 0.20
  set {double}($cmd+0x68) = -0.30
  set {unsigned int}($cmd+0x70) = 1
  set $rdi = 0
  set $rsi = $state
  set $rdx = $cmd
  set $pc = (void *)dyn_main
  continue
end
run
