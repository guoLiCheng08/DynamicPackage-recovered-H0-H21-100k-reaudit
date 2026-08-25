set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dynamics_flex_before_after_probe.log
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
  printf "entry data pointers\n"
  printf "a0=%p a1=%p a2=%p a3=%p a4=%p a5=%p\n", *(void **)($a0+8), *(void **)($a1+8), *(void **)($a2+8), *(void **)($a3+8), *(void **)($a4+8), *(void **)($a5+8)
  tbreak *(dynamics_flex+0xb43)
  continue
end
break *(dynamics_flex+0xb43)
commands
  silent
  printf "post data a0/a1\n"
  x/3gx *(void **)($a0+8)
  x/3gx *(void **)($a1+8)
  printf "post data a2/a3/a4/a5\n"
  x/10gx *(void **)($a2+8)
  x/10gx *(void **)($a3+8)
  x/10gx *(void **)($a4+8)
  x/10gx *(void **)($a5+8)
  kill
  quit
end
run
