set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $out_xyz=(double *)calloc(3,8)
  set $left_xyz=(double *)calloc(3,8)
  set $right_xyz=(double *)calloc(3,8)
  set $out=(char *)calloc(0x18,1)
  set $left=(char *)calloc(0x18,1)
  set $right=(char *)calloc(0x18,1)
  set $out_xyz[0]=101.0
  set $out_xyz[1]=102.0
  set $out_xyz[2]=103.0
  set $left_xyz[0]=1.25
  set $left_xyz[1]=-2.5
  set $left_xyz[2]=3.75
  set $right_xyz[0]=4.25
  set $right_xyz[1]=-5.5
  set $right_xyz[2]=6.75
  set *(double *)($out+0x00)=99.0
  set *(int *)($out+0x08)=3
  set *(int *)($out+0x0c)=0x11223344
  set *(void **)($out+0x10)=$out_xyz
  set *(double *)($left+0x00)=0.5
  set *(int *)($left+0x08)=3
  set *(int *)($left+0x0c)=0x55667788
  set *(void **)($left+0x10)=$left_xyz
  set *(double *)($right+0x00)=-1.5
  set *(int *)($right+0x08)=3
  set *(int *)($right+0x0c)=0x99aabbcc
  set *(void **)($right+0x10)=$right_xyz
  call ((void (*)(void *,void *,void *))quat_cross)($out,$left,$right)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_cross_p3_out_full.bin $out $out+0x18
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_cross_p3_out_xyz_full.bin $out_xyz $out_xyz+3
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_cross_p3_left_full.bin $left $left+0x18
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_cross_p3_left_xyz_full.bin $left_xyz $left_xyz+3
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_cross_p3_right_full.bin $right $right+0x18
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_cross_p3_right_xyz_full.bin $right_xyz $right_xyz+3
  kill
  quit
end
run
