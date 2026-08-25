set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $source_data = (double *)calloc(3,8)
  set $destination_data = (double *)calloc(3,8)
  set $destination_data[0] = 9.0
  set $destination_data[1] = 8.0
  set $destination_data[2] = 7.0
  set $source = (char *)calloc(0x10,1)
  set $destination = (char *)calloc(0x10,1)
  set *(int *)$source = 3
  set *(void **)($source+8) = $source_data
  set *(int *)$destination = 1
  set *(void **)($destination+8) = $destination_data
  set $results = (int *)calloc(2,4)
  set $results[0] = (int)vector_unit($destination,$source)
  set $source_data[0] = 3.0
  set $source_data[1] = 4.0
  set $source_data[2] = 12.0
  set $results[1] = (int)vector_unit($destination,$source)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_unit_nonfatal_p3_results.bin $results ($results+2)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_unit_nonfatal_p3_destination.bin $destination_data ($destination_data+3)
  kill
  quit
end
run
