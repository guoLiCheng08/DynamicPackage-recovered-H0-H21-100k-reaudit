set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/dynamics_flex_rhs_terms_real_model.log
set logging overwrite on
set logging enabled on

break *dynamics_flex
commands
  silent
  set $rate = *(double **)((char *)$rsi + 8)
  set $eta = *(double **)((char *)$rcx + 8)
  set $eta_dot = *(double **)((char *)$r9 + 8)
  set $rate[0] = 0.01
  set $rate[1] = -0.02
  set $rate[2] = 0.03
  set $eta[0] = 0.001
  set $eta[1] = -0.002
  set $eta[2] = 0.003
  set $eta_dot[0] = 0.011
  set $eta_dot[1] = -0.012
  set $eta_dot[2] = 0.013
  continue
end

break *(dynamics_flex + 1112)
commands
  silent
  printf "rhs term0 direct=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", *(double *)$rdi,*(double *)($rdi+8),*(double *)($rdi+16),*(unsigned long long *)$rdi,*(unsigned long long *)($rdi+8),*(unsigned long long *)($rdi+16)
  printf "rhs term1 direct=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", *(double *)$rsi,*(double *)($rsi+8),*(double *)($rsi+16),*(unsigned long long *)$rsi,*(unsigned long long *)($rsi+8),*(unsigned long long *)($rsi+16)
  printf "rhs term2 direct=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", *(double *)$rdx,*(double *)($rdx+8),*(double *)($rdx+16),*(unsigned long long *)$rdx,*(unsigned long long *)($rdx+8),*(unsigned long long *)($rdx+16)
  printf "rhs term3 direct=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", *(double *)$rax,*(double *)($rax+8),*(double *)($rax+16),*(unsigned long long *)$rax,*(unsigned long long *)($rax+8),*(unsigned long long *)($rax+16)
  continue
end

break *(dynamics_flex + 2315)
commands
  silent
  printf "final_rhs base=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", *(double *)$rdx,*(double *)($rdx+8),*(double *)($rdx+16),*(unsigned long long *)$rdx,*(unsigned long long *)($rdx+8),*(unsigned long long *)($rdx+16)
  printf "final_rhs minus=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", *(double *)$rsi,*(double *)($rsi+8),*(double *)($rsi+16),*(unsigned long long *)$rsi,*(unsigned long long *)($rsi+8),*(unsigned long long *)($rsi+16)
  printf "final_rhs plus=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", *(double *)$rcx,*(double *)($rcx+8),*(double *)($rcx+16),*(unsigned long long *)$rcx,*(unsigned long long *)($rcx+8),*(unsigned long long *)($rcx+16)
  printf "final_rhs stack_term=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", *(double *)($rsp+0x3e0),*(double *)($rsp+0x3e8),*(double *)($rsp+0x3f0),*(unsigned long long *)($rsp+0x3e0),*(unsigned long long *)($rsp+0x3e8),*(unsigned long long *)($rsp+0x3f0)
  continue
end

break *(dynamics_flex + 1813)
commands
  silent
  set $optional_matrix = *(double **)((char *)$rdi + 16)
  set $optional_input = *(double **)((char *)$rsi + 8)
  set $optional_output = *(double **)((char *)$rdx + 8)
  printf "optional_minus matrix_row0=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", $optional_matrix[0],$optional_matrix[1],$optional_matrix[2],*(unsigned long long *)($optional_matrix+0),*(unsigned long long *)($optional_matrix+1),*(unsigned long long *)($optional_matrix+2)
  printf "optional_minus input=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", $optional_input[0],$optional_input[1],$optional_input[2],*(unsigned long long *)($optional_input+0),*(unsigned long long *)($optional_input+1),*(unsigned long long *)($optional_input+2)
  printf "optional_minus output_pre=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", $optional_output[0],$optional_output[1],$optional_output[2],*(unsigned long long *)($optional_output+0),*(unsigned long long *)($optional_output+1),*(unsigned long long *)($optional_output+2)
  continue
end

break *(dynamics_flex + 1276)
commands
  silent
  set $cross_rate = *(double **)((char *)$rdi + 8)
  set $cross_momentum = *(double **)((char *)$rsi + 8)
  printf "cross rate=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", $cross_rate[0],$cross_rate[1],$cross_rate[2],*(unsigned long long *)($cross_rate+0),*(unsigned long long *)($cross_rate+1),*(unsigned long long *)($cross_rate+2)
  printf "cross momentum=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", $cross_momentum[0],$cross_momentum[1],$cross_momentum[2],*(unsigned long long *)($cross_momentum+0),*(unsigned long long *)($cross_momentum+1),*(unsigned long long *)($cross_momentum+2)
  continue
end

break *(dynamics_flex + 1312)
commands
  silent
  printf "rhs before_cross_sub=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", *(double *)$rax,*(double *)($rax+8),*(double *)($rax+16),*(unsigned long long *)$rax,*(unsigned long long *)($rax+8),*(unsigned long long *)($rax+16)
  printf "gyro cross=(%.17g,%.17g,%.17g) bits=(%016llx,%016llx,%016llx)\n", *(double *)$rdx,*(double *)($rdx+8),*(double *)($rdx+16),*(unsigned long long *)$rdx,*(unsigned long long *)($rdx+8),*(unsigned long long *)($rdx+16)
  continue
end

break *(dynamics_flex + 0xb36)
commands
  silent
  kill
  quit
end
run
