set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_print_p3_stdout_raw.txt
break main
commands
  silent
  set $xyz = (double *)calloc(3,8)
  set $xyz[0] = -2.5
  set $xyz[1] = 3.25
  set $xyz[2] = -4.5
  set $quat = (char *)calloc(0x18,1)
  set *(double *)$quat = 1.25
  set *(int *)($quat+8) = 3
  set *(void **)($quat+0x10) = $xyz
  set $result = (int *)calloc(1,4)
  set $result[0] = ((int (*)(void *))quat_print)($quat)
  call ((int (*)(void *))fflush)(0)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_print_p3_return.bin $result $result+1
  kill
  quit
end
run
