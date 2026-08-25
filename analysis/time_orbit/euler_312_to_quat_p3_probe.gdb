set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $xyz=(double *)calloc(3,8)
  set $quat=(char *)calloc(0x18,1)
  set *(int *)($quat+8)=3
  set *(void **)($quat+0x10)=$xyz
  call ((void (*)(void *,double,double,double))euler_angle_312_to_quat)($quat,0.3,-0.4,0.7)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_euler_312_to_quat_p3_output.bin $quat $quat+0x18
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_euler_312_to_quat_p3_xyz.bin $xyz $xyz+3
  kill
  quit
end
run
