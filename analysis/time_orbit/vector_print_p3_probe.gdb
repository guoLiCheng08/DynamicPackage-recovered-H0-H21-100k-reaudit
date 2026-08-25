set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_print_p3_stdout.txt
break main
commands
  silent
  set $data = (double *)calloc(3,8)
  set $data[0] = 1.0
  set $data[1] = -2.5
  set $data[2] = 3.25
  set $vector = (char *)calloc(0x10,1)
  set *(void **)($vector+8) = $data
  set *(int *)$vector = 0
  set $returns = (int *)calloc(2,4)
  set $returns[0] = ((int (*)(void *))vector_print)($vector)
  set *(int *)$vector = 3
  set $returns[1] = ((int (*)(void *))vector_print)($vector)
  call ((int (*)(void *))fflush)(0)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_print_p3_returns.bin $returns $returns+2
  kill
  quit
end
run
