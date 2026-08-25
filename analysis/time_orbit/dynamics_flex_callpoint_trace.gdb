set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/dynamics_flex_callpoint_trace.log
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

break *(dynamics_flex + 1924)
commands
  silent
  printf "CALL f0a4: MA * eta\n"
  info registers xmm0 xmm1
  x/4gx $rdi
  x/4gx $rsi
  x/4gx $rdx
  continue
end
break *(dynamics_flex + 2020)
commands
  silent
  printf "CALL f104: C * local3\n"
  info registers xmm0 xmm1
  x/4gx $rdi
  x/4gx $rsi
  x/4gx $rdx
  continue
end
break *(dynamics_flex + 2196)
commands
  silent
  printf "CALL f1b4: MD * eta_dot\n"
  info registers xmm0 xmm1
  x/4gx $rdi
  x/4gx $rsi
  x/4gx $rdx
  continue
end
break *(dynamics_flex + 2276)
commands
  silent
  printf "CALL f204: C * local3b\n"
  info registers xmm0 xmm1
  x/4gx $rdi
  x/4gx $rsi
  x/4gx $rdx
  continue
end
break *(dynamics_flex + 2591)
commands
  silent
  printf "CALL f33f: M4 * local3\n"
  info registers xmm0 xmm1
  x/4gx $rdi
  x/4gx $rsi
  x/4gx $rdx
  set $m4_input = *(double **)((char *)$rsi + 8)
  printf "M4_INPUT_VALUES\n"
  x/3fg $m4_input
  continue
end
break *(dynamics_flex + 2629)
commands
  silent
  printf "CALL f365: C_transpose * alpha\n"
  info registers xmm0 xmm1
  x/4gx $rdi
  x/4gx $rsi
  x/4gx $rdx
  continue
end
break *(dynamics_flex + 2731)
commands
  silent
  printf "CALL f3cb: MA * eta (second)\n"
  info registers xmm0 xmm1
  x/4gx $rdi
  x/4gx $rsi
  x/4gx $rdx
  continue
end
break *(dynamics_flex + 2854)
commands
  silent
  printf "CALL f446: MD * eta_dot (second)\n"
  info registers xmm0 xmm1
  x/4gx $rdi
  x/4gx $rsi
  x/4gx $rdx
  continue
end
break *(dynamics_flex + 0xb36)
commands
  silent
  kill
  quit
end
run
