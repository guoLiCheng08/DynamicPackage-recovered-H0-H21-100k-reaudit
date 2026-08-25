set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/quat_xi_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $q=(char *)calloc(1,24)
  set $qd=(double *)calloc(3,8)
  set $m=(char *)calloc(1,24)
  set $md=(double *)calloc(12,8)
  set *((double *)$q)=1.25
  set *((int *)($q+8))=3
  set *((void **)($q+16))=$qd
  set $qd[0]=-2.5
  set $qd[1]=3.75
  set $qd[2]=-4.5
  set *((int *)$m)=4
  set *((int *)($m+4))=3
  set *((int *)($m+8))=3
  set *((void **)($m+16))=$md
  call ((void (*)(void *, void *))quat_xi)((void *)$m,(void *)$q)
  printf "QUAT_XI_OUTPUT\n"
  x/12gx $md
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/quat_xi_gold.bin $md $md+12
  kill
  quit
end
run
