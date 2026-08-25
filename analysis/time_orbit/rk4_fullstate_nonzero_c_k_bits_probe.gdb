set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/rk4_fullstate_nonzero_c_k_bits_probe.log
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
  printf "pre_t_bits\n"
  x/1gx (double *)&t
  printf "pre_state_bits\n"
  x/33gx (double *)&y
  call ((void (*)(double)) RK4_Intergrator)(0.1)
  printf "step1_t_bits\n"
  x/1gx (double *)&t
  printf "step1_state_bits\n"
  x/33gx (double *)&y
  call ((void (*)(double)) RK4_Intergrator)(0.1)
  printf "step2_t_bits\n"
  x/1gx (double *)&t
  printf "step2_state_bits\n"
  x/33gx (double *)&y
  printf "H_w_B_bits\n"
  x/3gx *(void **)((char *)&H_w_B + 8)
  printf "L_c_B_bits\n"
  x/3gx *(void **)((char *)&L_c_B + 8)
  printf "J_c_B_bits\n"
  x/9gx *(void **)((char *)&J_c_B + 16)
  set $rk4_gold=(double *)&y
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/rk4_intergrator_gold.bin $rk4_gold $rk4_gold+33
  kill
  quit
end
run
