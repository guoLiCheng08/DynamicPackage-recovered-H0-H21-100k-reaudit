set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $data = (double *)calloc(3,8)
  set $data[0] = -2.0
  set $data[1] = 3.5
  set $data[2] = -1.0
  set $vector = (char *)calloc(0x10,1)
  set *(int *)$vector = 0
  set *(void **)($vector+8) = $data
  set $results = (double *)calloc(2,8)
  set $results[0] = vector_abs_max($vector)
  set *(int *)$vector = 3
  set $results[1] = vector_abs_max($vector)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_abs_max_nonfatal_p3_results.bin $results ($results+2)
  kill
  quit
end
run
