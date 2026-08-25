set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $left_data = (double *)calloc(3,8)
  set $right_data = (double *)calloc(3,8)
  set $out_data = (double *)calloc(3,8)
  set $left_data[0] = 1.0
  set $left_data[1] = 2.0
  set $left_data[2] = 3.0
  set $right_data[0] = -4.0
  set $right_data[1] = 5.0
  set $right_data[2] = -6.0
  set $out_data[0] = 9.0
  set $out_data[1] = 8.0
  set $out_data[2] = 7.0
  set $left = (char *)calloc(0x10,1)
  set $right = (char *)calloc(0x10,1)
  set $out = (char *)calloc(0x10,1)
  set *(int *)$left = 2
  set *(int *)$right = 3
  set *(int *)$out = 1
  set *(void **)($left+8) = $left_data
  set *(void **)($right+8) = $right_data
  set *(void **)($out+8) = $out_data
  set $results = (int *)calloc(2,4)
  set $results[0] = (int)vector3_cross($left,$right,$out)
  set *(int *)$right = 2
  set $results[1] = (int)vector3_cross($left,$right,$out)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector3_cross_nonfatal_p3_results.bin $results ($results+2)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector3_cross_nonfatal_p3_out.bin $out_data ($out_data+3)
  kill
  quit
end
run
