set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $data=(double *)calloc(81,8)
  set $matrix=(void *)calloc(1,24)
  set *(int *)$matrix=3
  set *(int *)($matrix+4)=3
  set *(int *)($matrix+8)=3
  set *(void **)($matrix+16)=$data
  call ((void (*)(void *,double))Rotation_X)($matrix,0.5)
  set *(void **)($matrix+16)=$data+9
  call ((void (*)(void *,double))Rotation_X)($matrix,-0.5)
  set *(void **)($matrix+16)=$data+18
  call ((void (*)(void *,double))Rotation_X)($matrix,0.0)
  set *(void **)($matrix+16)=$data+27
  call ((void (*)(void *,double))Rotation_Y)($matrix,0.5)
  set *(void **)($matrix+16)=$data+36
  call ((void (*)(void *,double))Rotation_Y)($matrix,-0.5)
  set *(void **)($matrix+16)=$data+45
  call ((void (*)(void *,double))Rotation_Y)($matrix,0.0)
  set *(void **)($matrix+16)=$data+54
  call ((void (*)(void *,double))Rotation_Z)($matrix,0.5)
  set *(void **)($matrix+16)=$data+63
  call ((void (*)(void *,double))Rotation_Z)($matrix,-0.5)
  set *(void **)($matrix+16)=$data+72
  call ((void (*)(void *,double))Rotation_Z)($matrix,0.0)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_rotation_xyz_p3_data.bin $data $data+81
  kill
  quit
end
run
