set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $xyz = (double *)calloc(3,8)
  set $xyz[0] = -0.5
  set $xyz[1] = 0.5
  set $xyz[2] = 0.5
  set $quat = (char *)calloc(0x18,1)
  set *(double *)$quat = 0.5
  set *(int *)($quat+8) = 3
  set *(int *)($quat+0xc) = 0
  set *(void **)($quat+0x10) = $xyz
  set $matrix_data = (double *)calloc(9,8)
  set $out = (char *)calloc(0x18,1)
  set *(int *)$out = 3
  set *(int *)($out+4) = 3
  set *(int *)($out+8) = 3
  set *(void **)($out+0x10) = $matrix_data
  call ((void (*)(void *,void *))quat2matrix)($out,$quat)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/quat2matrix_gold.bin $matrix_data $matrix_data+9
  kill
  quit
end
run
