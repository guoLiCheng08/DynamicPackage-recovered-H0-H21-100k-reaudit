set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/aerodynamic_torque_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  call (void)SatParaInit()
  set $out=(char *)calloc(1,16)
  set $outd=(double *)calloc(3,8)
  set *((int *)$out)=3
  set *((void **)($out+8))=$outd
  set $in1=(char *)calloc(1,16)
  set $in1d=(double *)calloc(3,8)
  set *((int *)$in1)=3
  set *((void **)($in1+8))=$in1d
  set $in1d[0]=1.0
  set $in1d[1]=-2.0
  set $in1d[2]=3.0
  set $in2=(char *)calloc(1,16)
  set $in2d=(double *)calloc(3,8)
  set *((int *)$in2)=3
  set *((void **)($in2+8))=$in2d
  set $in2d[0]=4.0
  set $in2d[1]=-5.0
  set $in2d[2]=6.0
  set $m=(char *)calloc(1,24)
  set $md=(double *)calloc(9,8)
  set *((int *)$m)=3
  set *((int *)($m+4))=3
  set *((int *)($m+8))=3
  set *((void **)($m+16))=$md
  set $md[0]=1.0
  set $md[4]=1.0
  set $md[8]=1.0
  call ((void (*)(void *, void *, void *, void *))AerodynamicTorque)((void *)$out,(void *)$in1,(void *)$in2,(void *)$m)
  printf "AERODYNAMIC_OUT\n"
  x/3gx $outd
  printf "AERODYNAMIC_IN1\n"
  x/3gx $in1d
  printf "AERODYNAMIC_IN2\n"
  x/3gx $in2d
  printf "AERODYNAMIC_M\n"
  x/9gx $md
  kill
  quit
end
run
