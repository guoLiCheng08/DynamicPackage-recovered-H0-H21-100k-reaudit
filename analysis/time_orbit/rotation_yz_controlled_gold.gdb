set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/rotation_yz_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $my=(char *)calloc(1,24)
  set $mz=(char *)calloc(1,24)
  set $yd=(double *)calloc(9,8)
  set $zd=(double *)calloc(9,8)
  set *((int *)$my)=3
  set *((int *)($my+4))=3
  set *((int *)($my+8))=3
  set *((void **)($my+16))=$yd
  set *((int *)$mz)=3
  set *((int *)($mz+4))=3
  set *((int *)($mz+8))=3
  set *((void **)($mz+16))=$zd
  call ((void (*)(void *, double))Rotation_Y)((void *)$my,1.1)
  call ((void (*)(void *, double))Rotation_Z)((void *)$mz,-0.7)
  printf "ROTATION_YZ_OUTPUTS\n"
  x/9gx $yd
  x/9gx $zd
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/rotationy_gold.bin $yd $yd+9
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/rotationz_gold.bin $zd $zd+9
  kill
  quit
end
run
