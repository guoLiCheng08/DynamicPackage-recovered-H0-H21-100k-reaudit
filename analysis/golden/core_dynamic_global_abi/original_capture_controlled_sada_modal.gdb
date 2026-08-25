set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/sada_modal_branch.log
set logging overwrite on
set logging enabled on
set $rk_stage = 0
set $rk_derivative_stage = 0
set $sada_modal_stage = 0
break *(dynamics_flex + 0x9df)
commands
  silent
  set $sada_input = *(double **)((char *)$rsi + 8)
  printf "sada_modal_in_stage=%d input=%016llx,%016llx,%016llx\n", $sada_modal_stage, *(unsigned long long *)$sada_input, *(unsigned long long *)($sada_input+1), *(unsigned long long *)($sada_input+2)
  continue
end
break *(dynamics_flex + 0xa24)
commands
  silent
  set $sada_output = *(double **)((char *)$r12 + 8)
  printf "sada_modal_out_stage=%d output=%016llx,%016llx,%016llx\n", $sada_modal_stage, *(unsigned long long *)$sada_output, *(unsigned long long *)($sada_output+1), *(unsigned long long *)($sada_output+2)
  set $sada_modal_stage = $sada_modal_stage + 1
  continue
end
set $flex_rhs_stage = 0
set $flex_momentum_stage = 0
set $flex_cross_stage = 0
break *(dynamics_flex + 1312)
commands
  silent
  printf "flex_cross_stage=%d rhs_before=%016llx,%016llx,%016llx gyro=%016llx,%016llx,%016llx\n", $flex_cross_stage, *(unsigned long long *)$rax, *(unsigned long long *)($rax+8), *(unsigned long long *)($rax+16), *(unsigned long long *)$rdx, *(unsigned long long *)($rdx+8), *(unsigned long long *)($rdx+16)
  set $flex_cross_stage = $flex_cross_stage + 1
  continue
end
break *(dynamics_flex + 1276)
commands
  silent
  set $cross_rate = *(double **)((char *)$rdi + 8)
  set $cross_momentum = *(double **)((char *)$rsi + 8)
  printf "flex_momentum_stage=%d rate=%016llx,%016llx,%016llx H=%016llx,%016llx,%016llx\n", $flex_momentum_stage, *(unsigned long long *)$cross_rate, *(unsigned long long *)($cross_rate+1), *(unsigned long long *)($cross_rate+2), *(unsigned long long *)$cross_momentum, *(unsigned long long *)($cross_momentum+1), *(unsigned long long *)($cross_momentum+2)
  set $flex_momentum_stage = $flex_momentum_stage + 1
  continue
end
break *(dynamics_flex + 2315)
commands
  silent
  printf "flex_rhs_stage=%d base=%016llx,%016llx,%016llx minus=%016llx,%016llx,%016llx plus=%016llx,%016llx,%016llx tail=%016llx,%016llx,%016llx\n", $flex_rhs_stage, *(unsigned long long *)$rdx, *(unsigned long long *)($rdx+8), *(unsigned long long *)($rdx+16), *(unsigned long long *)$rsi, *(unsigned long long *)($rsi+8), *(unsigned long long *)($rsi+16), *(unsigned long long *)$rcx, *(unsigned long long *)($rcx+8), *(unsigned long long *)($rcx+16), *(unsigned long long *)($rsp+0x3e0), *(unsigned long long *)($rsp+0x3e8), *(unsigned long long *)($rsp+0x3f0)
  printf "flex_rhs_value_stage=%d base=(%.17g,%.17g,%.17g) minus=(%.17g,%.17g,%.17g) plus=(%.17g,%.17g,%.17g) tail=(%.17g,%.17g,%.17g)\n", $flex_rhs_stage, *(double *)$rdx, *(double *)($rdx+8), *(double *)($rdx+16), *(double *)$rsi, *(double *)($rsi+8), *(double *)($rsi+16), *(double *)$rcx, *(double *)($rcx+8), *(double *)($rcx+16), *(double *)($rsp+0x3e0), *(double *)($rsp+0x3e8), *(double *)($rsp+0x3f0)
  set $flex_rhs_stage = $flex_rhs_stage + 1
  continue
end
break *differential_equation+0x645
commands
  silent
  printf "rk_derivative_stage=%d alpha=%016llx,%016llx,%016llx eta_ddot=%016llx,%016llx\n", $rk_derivative_stage, *(unsigned long long *)((double *)$rbx+4), *(unsigned long long *)((double *)$rbx+5), *(unsigned long long *)((double *)$rbx+6), *(unsigned long long *)((double *)$rbx+13), *(unsigned long long *)((double *)$rbx+22)
  set $rk_derivative_stage = $rk_derivative_stage + 1
  continue
end
break *differential_equation
commands
  silent
  printf "rk_stage=%d q=%016llx,%016llx,%016llx,%016llx w=%016llx,%016llx,%016llx r=%016llx,%016llx,%016llx v=%016llx,%016llx,%016llx flex=%016llx,%016llx H=%016llx,%016llx,%016llx L=%016llx,%016llx,%016llx F=%016llx,%016llx,%016llx\n", $rk_stage, *(unsigned long long *)$rsi, *(unsigned long long *)((double *)$rsi+1), *(unsigned long long *)((double *)$rsi+2), *(unsigned long long *)((double *)$rsi+3), *(unsigned long long *)((double *)$rsi+4), *(unsigned long long *)((double *)$rsi+5), *(unsigned long long *)((double *)$rsi+6), *(unsigned long long *)((double *)$rsi+7), *(unsigned long long *)((double *)$rsi+8), *(unsigned long long *)((double *)$rsi+9), *(unsigned long long *)((double *)$rsi+10), *(unsigned long long *)((double *)$rsi+11), *(unsigned long long *)((double *)$rsi+12), *(unsigned long long *)((double *)$rsi+13), *(unsigned long long *)((double *)$rsi+32), *(unsigned long long *)*(void **)((char *)&H_w_B+8), *(unsigned long long *)((double *)*(void **)((char *)&H_w_B+8)+1), *(unsigned long long *)((double *)*(void **)((char *)&H_w_B+8)+2), *(unsigned long long *)*(void **)((char *)&L_c_B+8), *(unsigned long long *)((double *)*(void **)((char *)&L_c_B+8)+1), *(unsigned long long *)((double *)*(void **)((char *)&L_c_B+8)+2), *(unsigned long long *)*(void **)((char *)&F_I_external+8), *(unsigned long long *)((double *)*(void **)((char *)&F_I_external+8)+1), *(unsigned long long *)((double *)*(void **)((char *)&F_I_external+8)+2)
  set $rk_stage = $rk_stage + 1
  continue
end
break *CoreDynamic
commands
  silent
  set $out_entry = $rdi
  set $input_entry = $rsi
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
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/pre_input_90.bin $input_entry ($input_entry+0x90)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/pre_output_148.bin $out_entry ($out_entry+0x148)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/pre_y_33.bin $y_begin $y_end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/pre_t.bin $t_begin $t_end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/step_time.bin &step_time ((char *)&step_time+8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/pre_sat_1078.bin &Sat ((char *)&Sat+0x1078)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/pre_sat_torque_168.bin &SatTorque ((char *)&SatTorque+0x168)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/j_c_b_data.bin $jdata ((char *)$jdata+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/j_c_b_inv_data.bin $jidata ((char *)$jidata+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/h_w_b_data.bin $hdata ((char *)$hdata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/l_c_b_data.bin $ldata ((char *)$ldata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/f_i_external_data.bin $fdata ((char *)$fdata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/spacecraft_mass.bin &SpacecraftMass ((char *)&SpacecraftMass+8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/sada_raw.bin &SADA ((char *)&SADA+104)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/sat_m4a8_data.bin $m4a8_data ((char *)$m4a8_data+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/sat_m688_data.bin $m688_data ((char *)$m688_data+240)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/sat_coupling_data.bin $coupling_data ((char *)$coupling_data+240)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/sat_m9_data.bin $m9_data ((char *)$m9_data+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/sat_modal_a_data.bin $ma_data ((char *)$ma_data+800)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/sat_modal_d_data.bin $md_data ((char *)$md_data+800)
  printf "core_entry out=%p input=%p y0=%.17g\n", $out_entry, $input_entry, ((double *)&y)[0]
  continue
end
break *CoreDynamic+0x700
commands
  silent
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/post_output_148.bin $out_entry ($out_entry+0x148)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/post_y_33.bin $y_begin $y_end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/post_t.bin $t_begin $t_end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/post_sat_1078.bin &Sat ((char *)&Sat+0x1078)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/post_sat_torque_168.bin &SatTorque ((char *)&SatTorque+0x168)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/post_h_w_b_data.bin $hdata ((char *)$hdata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/post_l_c_b_data.bin $ldata ((char *)$ldata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/core_dynamic_global_abi/controlled_input/post_f_i_external_data.bin $fdata ((char *)$fdata+24)
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
  set $state[0] = 1.0
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
