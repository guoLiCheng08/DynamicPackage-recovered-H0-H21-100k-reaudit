set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $xyz0=(double *)calloc(3,8)
  set $q0=(void *)calloc(1,32)
  set *(double *)$q0=1.0
  set *(int *)($q0+8)=3
  set *(void **)($q0+16)=$xyz0
  set $xyz1=(double *)calloc(3,8)
  set $q1=(void *)calloc(1,32)
  set *(double *)$q1=0.7071067811865476
  set *(int *)($q1+8)=3
  set *(void **)($q1+16)=$xyz1
  set $xyz1[2]=0.7071067811865476
  set $xyz2=(double *)calloc(3,8)
  set $q2=(void *)calloc(1,32)
  set *(int *)($q2+8)=3
  set *(void **)($q2+16)=$xyz2
  call ((void (*)(void *,double,double,double))euler_angle_312_to_quat)($q2,0.3,-0.4,0.5)
  set $xyz3=(double *)calloc(3,8)
  set $q3=(void *)calloc(1,32)
  set *(double *)$q3=*(double *)$q2*2.0
  set *(int *)($q3+8)=3
  set *(void **)($q3+16)=$xyz3
  set $xyz3[0]=$xyz2[0]*2.0
  set $xyz3[1]=$xyz2[1]*2.0
  set $xyz3[2]=$xyz2[2]*2.0
  set $out=(double *)calloc(12,8)
  call ((void (*)(double *,double *,double *,const void *))quat_to_euler_angle_312)($out,$out+1,$out+2,$q0)
  call ((void (*)(double *,double *,double *,const void *))quat_to_euler_angle_312)($out+3,$out+4,$out+5,$q1)
  call ((void (*)(double *,double *,double *,const void *))quat_to_euler_angle_312)($out+6,$out+7,$out+8,$q2)
  call ((void (*)(double *,double *,double *,const void *))quat_to_euler_angle_312)($out+9,$out+10,$out+11,$q3)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/quat_to_euler_angle_312_gold.bin $out $out+12
  kill
  quit
end
run
