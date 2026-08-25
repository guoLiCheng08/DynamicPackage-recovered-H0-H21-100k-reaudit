set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $left_data = (double *)calloc(8,8)
  set $right_data = (double *)calloc(10,8)
  set $left_data[0] = 11.0
  set $left_data[1] = 22.0
  set $left_data[2] = 90.0
  set $left_data[3] = 53.0
  set $left_data[4] = 64.0
  set $left_data[5] = 91.0
  set $right_data[0] = 10.0
  set $right_data[1] = 20.0
  set $right_data[2] = 30.0
  set $right_data[3] = 40.0
  set $right_data[4] = 50.0
  set $right_data[5] = 60.0
  set $right_data[6] = 70.0
  set $right_data[7] = 80.0
  set $left = (char *)calloc(0x18,1)
  set $right = (char *)calloc(0x18,1)
  set *(int *)$left = 2
  set *(int *)($left+4) = 2
  set *(int *)($left+8) = 3
  set *(void **)($left+0x10) = $left_data
  set *(int *)$right = 3
  set *(int *)($right+4) = 2
  set *(int *)($right+8) = 4
  set *(void **)($right+0x10) = $right_data
  set $results = (int *)calloc(2,4)
  set $results[0] = (int)matrix_sub($left,$right)
  set *(int *)$right = 2
  set $results[1] = (int)matrix_sub($left,$right)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_matrix_sub_nonfatal_p3_results.bin $results ($results+2)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_matrix_sub_nonfatal_p3_left.bin $left_data ($left_data+8)
  kill
  quit
end
run
