set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $left_data = (double *)calloc(3,8)
  set $right_data = (double *)calloc(3,8)
  set $left_data[0] = 1.5
  set $left_data[1] = -2.0
  set $left_data[2] = 4.0
  set $right_data[0] = -3.0
  set $right_data[1] = 0.25
  set $right_data[2] = 2.0
  set $left = (char *)calloc(0x10,1)
  set $right = (char *)calloc(0x10,1)
  set *(int *)$left = 2
  set *(int *)$right = 3
  set *(void **)($left+8) = $left_data
  set *(void **)($right+8) = $right_data
  set $out = (double *)calloc(1,8)
  set $out[0] = 123.5
  set $results = (int *)calloc(3,4)
  set $results[0] = (int)vector_dot($left,$right,$out)
  set $snapshots = (double *)calloc(3,8)
  set $snapshots[0] = $out[0]
  set *(int *)$left = 0
  set *(int *)$right = 0
  set $results[1] = (int)vector_dot($left,$right,$out)
  set $snapshots[1] = $out[0]
  set *(int *)$left = 3
  set *(int *)$right = 3
  set $results[2] = (int)vector_dot($left,$right,$out)
  set $snapshots[2] = $out[0]
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_dot_nonfatal_p3_results.bin $results ($results+3)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_dot_nonfatal_p3_snapshots.bin $snapshots ($snapshots+3)
  kill
  quit
end
run
