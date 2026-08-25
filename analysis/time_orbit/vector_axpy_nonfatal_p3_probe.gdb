set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $source_data = (double *)calloc(3,8)
  set $destination_data = (double *)calloc(3,8)
  set $source_data[0] = 1.5
  set $source_data[1] = -2.0
  set $source_data[2] = 0.25
  set $destination_data[0] = 10.0
  set $destination_data[1] = 20.0
  set $destination_data[2] = -30.0
  set $source = (char *)calloc(16,1)
  set $destination = (char *)calloc(16,1)
  set {int}($source+0) = 3
  set {int}($destination+0) = 2
  set {void *}($source+8) = $source_data
  set {void *}($destination+8) = $destination_data
  set $result = (int *)calloc(3,4)
  set $result[0] = ((int (*)(void *,double,void *))vector_axpy)($source,0.0,$destination)
  set {int}($source+0) = 0
  set $result[1] = ((int (*)(void *,double,void *))vector_axpy)($source,2.0,$destination)
  set {int}($source+0) = 3
  set {int}($destination+0) = 2
  set $result[2] = ((int (*)(void *,double,void *))vector_axpy)($source,-0.5,$destination)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_axpy_nonfatal_p3_result.bin $result ($result+3)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_axpy_nonfatal_p3_destination.bin $destination_data ($destination_data+3)
  kill
  quit
end
run
