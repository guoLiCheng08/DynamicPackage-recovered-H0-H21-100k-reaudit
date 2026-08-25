set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $data = (double *)calloc(12,8)
  set $source_data = (double *)calloc(3,8)
  set $index = 0
  while $index < 12
    set $data[$index] = $index + 1.0
    set $index = $index + 1
  end
  set $source_data[0] = 10.0
  set $source_data[1] = 20.0
  set $source_data[2] = 30.0
  set $matrix = (char *)calloc(0x18,1)
  set $source = (char *)calloc(0x10,1)
  set *(int *)$matrix = 2
  set *(int *)($matrix+4) = 2
  set *(int *)($matrix+8) = 4
  set *(void **)($matrix+0x10) = $data
  set *(int *)$source = 3
  set *(void **)($source+8) = $source_data
  set $results = (int *)calloc(3,4)
  set $results[0] = (int)matrix_set_col($matrix,2,$source)
  set $results[1] = (int)matrix_set_col($matrix,1,$source)
  set *(int *)$source = 2
  set $results[2] = (int)matrix_set_col($matrix,1,$source)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_matrix_set_col_nonfatal_p3_results.bin $results ($results+3)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_matrix_set_col_nonfatal_p3_data.bin $data ($data+12)
  kill
  quit
end
run
