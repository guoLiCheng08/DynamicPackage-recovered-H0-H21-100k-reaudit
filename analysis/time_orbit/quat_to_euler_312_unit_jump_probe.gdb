set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $xyz=(double *)calloc(3,8)
  set $q=(void *)calloc(1,32)
  set *(double *)$q=1.0
  set *(int *)($q+8)=3
  set *(void **)($q+16)=$xyz
  set $out=(double *)calloc(3,8)
  set $rdi=$out
  set $rsi=$out+1
  set $rdx=$out+2
  set $rcx=$q
  set $rip=quat_to_euler_angle_312
  continue
end
break *quat_to_euler_angle_312+0x11d
commands
  silent
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_to_euler_312_unit_jump.bin $out $out+3
  x/3gx $out
  kill
  quit
end
run
