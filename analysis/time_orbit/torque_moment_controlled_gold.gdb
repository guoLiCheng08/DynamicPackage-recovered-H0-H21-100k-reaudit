set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/torque_moment_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $tin=(char *)calloc(1,16)
  set $tout=(char *)calloc(1,16)
  set $hin=(char *)calloc(1,16)
  set $hout=(char *)calloc(1,16)
  set $td=(double *)calloc(3,8)
  set $tod=(double *)calloc(3,8)
  set $hd=(double *)calloc(3,8)
  set $hod=(double *)calloc(3,8)
  set *((int *)$tin)=3
  set *((void **)($tin+8))=$td
  set *((int *)$tout)=3
  set *((void **)($tout+8))=$tod
  set *((int *)$hin)=3
  set *((void **)($hin+8))=$hd
  set *((int *)$hout)=3
  set *((void **)($hout+8))=$hod
  set $td[0]=1.25
  set $td[1]=-2.5
  set $td[2]=3.75
  set $hd[0]=-4.5
  set $hd[1]=5.25
  set $hd[2]=-6.75
  call ((void (*)(void *))SetTorque)((void *)$tin)
  call ((void (*)(void *))GetTorque)((void *)$tout)
  call ((void (*)(void *))SetWheelAngularMoment)((void *)$hin)
  call ((void (*)(void *))GetWheelAngularMoment)((void *)$hout)
  printf "TORQUE_MOMENT_OUTPUTS\n"
  x/3gx $tod
  x/3gx $hod
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/settorque_gold.bin $tod $tod+3
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gettorque_gold.bin $tod $tod+3
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/setwheelangularmoment_gold.bin $hod $hod+3
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/getwheelangularmoment_gold.bin $hod $hod+3
  kill
  quit
end
run
