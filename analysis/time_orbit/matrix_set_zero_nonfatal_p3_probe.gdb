set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $data = (double *)calloc(12,8)
  set $index = 0
  while $index < 12
    set $data[$index] = $index + 1.0
    set $index = $index + 1
  end
  set $matrix = (char *)calloc(0x18,1)
  set *(int *)$matrix = 2
  set *(int *)($matrix+4) = 2
  set *(int *)($matrix+8) = 4
  set *(void **)($matrix+0x10) = $data
  set $result = (int)matrix_set_zero($matrix)
  set $result_blob = (int *)calloc(1,4)
  set $result_blob[0] = $result
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_matrix_set_zero_nonfatal_p3_result.bin $result_blob ($result_blob+1)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_matrix_set_zero_nonfatal_p3_data.bin $data ($data+12)
  kill
  quit
end
run
