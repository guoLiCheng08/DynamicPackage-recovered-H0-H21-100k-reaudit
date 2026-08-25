set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/frontalarea_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $v=(char *)calloc(1,16)
  set $vd=(double *)calloc(3,8)
  set *((int *)$v)=3
  set *((void **)($v+8))=$vd
  set $vd[0]=1.0
  set $vd[1]=-2.0
  set $vd[2]=3.0
  set $m1=(char *)calloc(1,24)
  set $m1d=(double *)calloc(9,8)
  set *((int *)$m1)=3
  set *((int *)($m1+4))=3
  set *((int *)($m1+8))=3
  set *((void **)($m1+16))=$m1d
  set $m1d[0]=1.0
  set $m1d[4]=1.0
  set $m1d[8]=1.0
  set $m2=(char *)calloc(1,24)
  set $m2d=(double *)calloc(9,8)
  set *((int *)$m2)=3
  set *((int *)($m2+4))=3
  set *((int *)($m2+8))=3
  set *((void **)($m2+16))=$m2d
  set $m2d[0]=2.0
  set $m2d[4]=3.0
  set $m2d[8]=4.0
  set $result=((int (*)(void *, void *, void *, double))FrontalArea)((void *)$v,(void *)$m1,(void *)$m2,9.25)
  printf "FRONTALAREA_RETURN=%d\n", $result
  printf "FRONTALAREA_INPUT_VECTOR\n"
  x/3gx $vd
  printf "FRONTALAREA_INPUT_M1\n"
  x/9gx $m1d
  printf "FRONTALAREA_INPUT_M2\n"
  x/9gx $m2d
  kill
  quit
end
run
