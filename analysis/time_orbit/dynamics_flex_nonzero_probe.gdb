set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dynamics_flex_nonzero_probe.log
set logging overwrite on
set logging enabled on
break *dynamics_flex
commands
  silent
  set $a0 = $rdi
  set $a1 = $rsi
  set $a2 = $rdx
  set $a3 = $rcx
  set $a4 = $r8
  set $a5 = $r9
  set *(double *)(*(void **)($a0+8)+0) = 0.1
  set *(double *)(*(void **)($a0+8)+8) = -0.2
  set *(double *)(*(void **)($a0+8)+16) = 0.3
  set *(double *)(*(void **)($a1+8)+0) = 1.0
  set *(double *)(*(void **)($a1+8)+8) = 2.0
  set *(double *)(*(void **)($a1+8)+16) = -1.0
  set *(double *)(*(void **)($a3+8)+0) = 0.25
  set *(double *)(*(void **)($a3+8)+8) = -0.5
  set *(double *)(*(void **)($a5+8)+0) = 0.75
  set *(double *)(*(void **)($a5+8)+8) = 0.125
  tbreak *(dynamics_flex+0xb43)
  continue
end
break *(dynamics_flex+0xb43)
commands
  silent
  printf "nonzero post a0/a1/a2/a3/a4/a5\n"
  x/3gx *(void **)($a0+8)
  x/3gx *(void **)($a1+8)
  x/10gx *(void **)($a2+8)
  x/10gx *(void **)($a3+8)
  x/10gx *(void **)($a4+8)
  x/10gx *(void **)($a5+8)
  kill
  quit
end
run
