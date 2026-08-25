set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $mdata=(double *)calloc(9,8)
  set $mdata[0]=1.0
  set $mdata[4]=1.0
  set $mdata[8]=1.0
  set $mat=(char *)calloc(0x18,1)
  set *(int *)$mat=3
  set *(int *)($mat+4)=3
  set *(int *)($mat+8)=3
  set *(void **)($mat+0x10)=$mdata
  set $xyz=(double *)calloc(3,8)
  set $quat=(char *)calloc(0x18,1)
  set *(int *)($quat+8)=3
  set *(void **)($quat+0x10)=$xyz
  call ((void (*)(void *,void *))matrix2quat)($quat,$mat)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_matrix2quat_identity_p3_quat.bin $quat $quat+0x18
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_matrix2quat_identity_p3_xyz.bin $xyz $xyz+3
  kill
  quit
end
run
