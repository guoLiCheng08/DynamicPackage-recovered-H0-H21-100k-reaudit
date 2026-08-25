set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $left_data = (double *)calloc(3,8)
  set $right_data = (double *)calloc(3,8)
  set $left_data[0] = 5.0
  set $left_data[1] = 3.0
  set $left_data[2] = -2.5
  set $right_data[0] = 1.0
  set $right_data[1] = -4.0
  set $right_data[2] = 0.5
  set $left = (char *)calloc(0x10,1)
  set $right = (char *)calloc(0x10,1)
  set *(int *)$left = 2
  set *(int *)$right = 3
  set *(void **)($left+8) = $left_data
  set *(void **)($right+8) = $right_data
  set $results = (int *)calloc(3,4)
  set $results[0] = (int)vector_sub($left,$right)
  set *(int *)$left = 0
  set *(int *)$right = 0
  set $results[1] = (int)vector_sub($left,$right)
  set *(int *)$left = 3
  set *(int *)$right = 3
  set $results[2] = (int)vector_sub($left,$right)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_sub_nonfatal_p3_results.bin $results ($results+3)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_sub_nonfatal_p3_left.bin $left_data ($left_data+3)
  kill
  quit
end
run
