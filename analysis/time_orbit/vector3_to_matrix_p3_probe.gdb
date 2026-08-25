set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $data=(double *)calloc(12,8)
  set $matrix=(void *)calloc(1,24)
  set *(int *)$matrix=3
  set *(int *)($matrix+4)=3
  set *(int *)($matrix+8)=4
  set *(void **)($matrix+16)=$data
  set $data[0]=1.0
  set $data[1]=-2.0
  set $data[2]=3.0
  set $data[3]=99.0
  set $data[4]=4.0
  set $data[5]=-5.0
  set $data[6]=6.0
  set $data[7]=98.0
  set $data[8]=7.0
  set $data[9]=-8.0
  set $data[10]=9.0
  set $data[11]=97.0
  call ((void (*)(void *))vector3_to_matrix)($matrix)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector3_to_matrix_p3_data.bin $data $data+12
  kill
  quit
end
run
