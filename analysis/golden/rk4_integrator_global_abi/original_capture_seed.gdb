set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/original_capture.log
set logging overwrite on
set logging enabled on
dprintf *differential_equation, "deq q_bits=%016llx,%016llx,%016llx,%016llx rate_bits=%016llx,%016llx,%016llx\\n", *(unsigned long long *)$rsi, *(unsigned long long *)((double *)$rsi+1), *(unsigned long long *)((double *)$rsi+2), *(unsigned long long *)((double *)$rsi+3), *(unsigned long long *)((double *)$rsi+4), *(unsigned long long *)((double *)$rsi+5), *(unsigned long long *)((double *)$rsi+6)
break main
commands
  silent
  set $initial = (double *)calloc(1, 232)
  set $initial[0] = 0.1
  set $initial[7] = 2020.0
  set $initial[8] = 1.0
  set $initial[9] = 2.0
  set $initial[10] = 3.0
  set $initial[11] = 4.0
  set $initial[12] = 5.0
  set $initial[13] = 7000000.0
  set $initial[14] = 0.001
  set $initial[15] = 0.1
  set $initial[16] = 0.2
  set $initial[17] = 0.3
  set $initial[18] = 0.4
  set $initial[19] = 10.0
  set $initial[23] = 11.0
  set $initial[27] = 12.0
  set $initial[28] = 100.0
  call ((void (*)(void *)) DynamicInit)($initial)
  set $c_data = *(double **)((char *)&Sat + 0x8a8)
  set $c_data[0] = 0.1
  set $c_data[10] = -0.05
  set $c_data[20] = 0.025
  set $matrix_a_data = *(double **)((char *)&Sat + 0xa10)
  set $matrix_d_data = *(double **)((char *)&Sat + 0xd48)
  set $matrix_a_data[0] = 2.0
  set $matrix_d_data[0] = 3.0
  set ((double *)&y)[0] = 1.0
  set ((double *)&y)[1] = 0.0
  set ((double *)&y)[2] = 0.0
  set ((double *)&y)[3] = 0.0
  set ((double *)&y)[4] = 0.01
  set ((double *)&y)[5] = -0.02
  set ((double *)&y)[6] = 0.03
  set ((double *)&y)[13] = 0.001
  set ((double *)&y)[14] = -0.002
  set ((double *)&y)[15] = 0.003
  set ((double *)&y)[16] = -0.004
  set ((double *)&y)[17] = 0.005
  set ((double *)&y)[18] = -0.006
  set ((double *)&y)[19] = 0.007
  set ((double *)&y)[20] = -0.008
  set ((double *)&y)[21] = 0.009
  set ((double *)&y)[22] = -0.010
  set ((double *)&y)[23] = 0.011
  set ((double *)&y)[24] = -0.012
  set ((double *)&y)[25] = 0.013
  set ((double *)&y)[26] = -0.014
  set ((double *)&y)[27] = 0.015
  set ((double *)&y)[28] = -0.016
  set ((double *)&y)[29] = 0.017
  set ((double *)&y)[30] = -0.018
  set ((double *)&y)[31] = 0.019
  set ((double *)&y)[32] = -0.020
  set $state_begin = (char *)&y
  set $state_end = $state_begin + 264
  set $time_begin = (char *)&t
  set $time_end = $time_begin + 8
  set $step_mem = (double *)malloc(8)
  set $step_mem[0] = 0.1
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/pre_state_33.bin $state_begin $state_end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/pre_t.bin $time_begin $time_end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/explicit_step.bin $step_mem ((char *)$step_mem+8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/step_time.bin &step_time ((char *)&step_time+8)
  set $hdata = *(void **)((char *)&H_w_B+8)
  set $ldata = *(void **)((char *)&L_c_B+8)
  set $jdata = *(void **)((char *)&J_c_B+16)
  set $jidata = *(void **)((char *)&J_c_B_inv+16)
  set $fdata = *(void **)((char *)&F_I_external+8)
  set $m4a8_data = *(void **)((char *)&Sat+0x4b8)
  set $m688_data = *(void **)((char *)&Sat+0x698)
  set $coupling_data = *(void **)((char *)&Sat+0x8a8)
  set $m9_data = *(void **)((char *)&Sat+0x9b0)
  set $ma_data2 = *(void **)((char *)&Sat+0xa10)
  set $md_data2 = *(void **)((char *)&Sat+0xd48)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/h_w_b_data.bin $hdata ((char *)$hdata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/l_c_b_data.bin $ldata ((char *)$ldata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/j_c_b_data.bin $jdata ((char *)$jdata+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/j_c_b_inv_data.bin $jidata ((char *)$jidata+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/f_i_external_data.bin $fdata ((char *)$fdata+24)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/spacecraft_mass.bin &SpacecraftMass ((char *)&SpacecraftMass+8)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/sada_raw.bin &SADA ((char *)&SADA+104)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/sat_m4a8_data.bin $m4a8_data ((char *)$m4a8_data+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/sat_m688_data.bin $m688_data ((char *)$m688_data+240)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/sat_coupling_data.bin $coupling_data ((char *)$coupling_data+240)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/sat_m9_data.bin $m9_data ((char *)$m9_data+72)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/sat_modal_a_data.bin $ma_data2 ((char *)$ma_data2+800)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/sat_modal_d_data.bin $md_data2 ((char *)$md_data2+800)
  call ((void (*)(double)) RK4_Intergrator)($step_mem[0])
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/step1_state_33.bin $state_begin $state_end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/step1_t.bin $time_begin $time_end
  call ((void (*)(double)) RK4_Intergrator)($step_mem[0])
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/step2_state_33.bin $state_begin $state_end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/golden/rk4_integrator_global_abi/step2_t.bin $time_begin $time_end
  printf "pre_t_bits\n"
  x/1gx (double *)&t
  printf "step2_t_bits\n"
  x/1gx (double *)&t
  printf "step2_state_bits\n"
  x/33gx (double *)&y
  kill
  quit
end
run
