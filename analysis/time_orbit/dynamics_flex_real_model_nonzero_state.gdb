set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/dynamics_flex_real_model_nonzero_state.log
set logging overwrite on
set logging enabled on
set $seen_entry = 0
break *dynamics_flex
commands
  silent
  set $seen_entry = 1
  set $out_rigid = $rdi
  set $out_modal_a = $rdx
  set $out_modal_v = $r8
  set $state_rate = $rsi
  set $state_modal_a = $rcx
  set $state_modal_v = $r9
  set $rate_data = *(double **)((char *)$state_rate + 8)
  set $modal_a_data = *(double **)((char *)$state_modal_a + 8)
  set $modal_v_data = *(double **)((char *)$state_modal_v + 8)
  set $rate_data[0] = 0.01
  set $rate_data[1] = -0.02
  set $rate_data[2] = 0.03
  set $modal_a_data[0] = 0.001
  set $modal_a_data[1] = -0.002
  set $modal_a_data[2] = 0.003
  set $modal_v_data[0] = 0.011
  set $modal_v_data[1] = -0.012
  set $modal_v_data[2] = 0.013
  continue
end
break *(dynamics_flex + 0x958)
commands
  silent
  set $effective_data = (double *)$rdi
  printf "effective row0=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", $effective_data[0],$effective_data[1],$effective_data[2],*(unsigned long long *)($effective_data+0),*(unsigned long long *)($effective_data+1),*(unsigned long long *)($effective_data+2)
  continue
end
break *(dynamics_flex + 0x988)
commands
  silent
  set $rhs_vector = $rsi
  set $inverse_matrix = $rdi
  set $alpha_vector = $rdx
  set $rhs_data = *(double **)((char *)$rhs_vector + 8)
  set $inverse_data = *(double **)((char *)$inverse_matrix + 16)
  printf "pre_solve rhs=(%.17g,%.17g,%.17g) rhs_bits=(%016llx,%016llx,%016llx) inv_row0=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", $rhs_data[0],$rhs_data[1],$rhs_data[2],*(unsigned long long *)($rhs_data+0),*(unsigned long long *)($rhs_data+1),*(unsigned long long *)($rhs_data+2),$inverse_data[0],$inverse_data[1],$inverse_data[2],*(unsigned long long *)($inverse_data+0),*(unsigned long long *)($inverse_data+1),*(unsigned long long *)($inverse_data+2)
  continue
end
break *(dynamics_flex + 0xb36)
commands
  silent
  if $seen_entry == 1
    printf "state rate=(%.17g,%.17g,%.17g) modal_a0=%.17g modal_v0=%.17g\n", *(double *)*(void **)((char *)$state_rate+8), *(double *)(*(void **)((char *)$state_rate+8)+8), *(double *)(*(void **)((char *)$state_rate+8)+16), *(double *)*(void **)((char *)$state_modal_a+8), *(double *)*(void **)((char *)$state_modal_v+8)
    set $out_rigid_data = *(double **)((char *)$out_rigid + 8)
    set $out_modal_a_data = *(double **)((char *)$out_modal_a + 8)
    printf "out rigid=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", $out_rigid_data[0], $out_rigid_data[1], $out_rigid_data[2], *(unsigned long long *)$out_rigid_data, *(unsigned long long *)($out_rigid_data+1), *(unsigned long long *)($out_rigid_data+2)
    printf "out modal_a[0..4]=(%.17g,%.17g,%.17g,%.17g,%.17g)\n", $out_modal_a_data[0],$out_modal_a_data[1],$out_modal_a_data[2],$out_modal_a_data[3],$out_modal_a_data[4]
    printf "out modal_a[5..9]=(%.17g,%.17g,%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx,%016llx,%016llx,%016llx,%016llx,%016llx,%016llx,%016llx)\n", $out_modal_a_data[5],$out_modal_a_data[6],$out_modal_a_data[7],$out_modal_a_data[8],$out_modal_a_data[9], *(unsigned long long *)($out_modal_a_data+0),*(unsigned long long *)($out_modal_a_data+1),*(unsigned long long *)($out_modal_a_data+2),*(unsigned long long *)($out_modal_a_data+3),*(unsigned long long *)($out_modal_a_data+4),*(unsigned long long *)($out_modal_a_data+5),*(unsigned long long *)($out_modal_a_data+6),*(unsigned long long *)($out_modal_a_data+7),*(unsigned long long *)($out_modal_a_data+8),*(unsigned long long *)($out_modal_a_data+9)
    printf "Sat C first=(%.17g,%.17g,%.17g) K00=%.17g D00=%.17g\n", *(double *)*(void **)((char *)&Sat+0x8a8), *(double *)(*(void **)((char *)&Sat+0x8a8)+8), *(double *)(*(void **)((char *)&Sat+0x8a8)+16), *(double *)*(void **)((char *)&Sat+0xa10), *(double *)*(void **)((char *)&Sat+0xd48)
    kill
    quit
  end
  continue
end
run
