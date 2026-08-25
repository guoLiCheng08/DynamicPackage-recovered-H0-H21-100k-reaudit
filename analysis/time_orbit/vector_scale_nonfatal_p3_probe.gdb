set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $data = (double *)calloc(3,8)
  set $data[0] = 1.5
  set $data[1] = -2.0
  set $data[2] = 0.0
  set $vector = (char *)calloc(0x10,1)
  set *(int *)$vector = 0
  set *(void **)($vector+8) = $data
  set $results = (int *)calloc(2,4)
  set $results[0] = (int)vector_scale($vector,-3.0)
  set *(int *)$vector = 3
  set $results[1] = (int)vector_scale($vector,-3.0)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_scale_nonfatal_p3_results.bin $results ($results+2)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_scale_nonfatal_p3_data.bin $data ($data+3)
  kill
  quit
end
run
