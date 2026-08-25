set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $mdata=(double *)calloc(36,8)
  set $i=0
  while $i<36
    set $mdata[$i]=0.0
    set $i=$i+1
  end
  set $mdata[0]=1.0
  set $mdata[4]=-1.0
  set $mdata[8]=-1.0
  set $mdata[9]=-1.0
  set $mdata[13]=1.0
  set $mdata[17]=-1.0
  set $mdata[18]=-1.0
  set $mdata[22]=-1.0
  set $mdata[26]=1.0
  set $mat=(char *)calloc(0x18,1)
  set *(int *)$mat=3
  set *(int *)($mat+4)=3
  set *(int *)($mat+8)=3
  set $xyz=(double *)calloc(9,8)
  set $quat=(char *)calloc(0x48,1)
  set $i=0
  while $i<3
    set *(int *)($quat+$i*0x18+8)=3
    set *(void **)($quat+$i*0x18+0x10)=$xyz+$i*3
    set *(void **)($mat+0x10)=$mdata+$i*9
    call ((void (*)(void *,void *))matrix2quat)($quat+$i*0x18,$mat)
    set $i=$i+1
  end
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_matrix2quat_axis_p3_quat.bin $quat $quat+0x48
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_matrix2quat_axis_p3_xyz.bin $xyz $xyz+9
  kill
  quit
end
run
