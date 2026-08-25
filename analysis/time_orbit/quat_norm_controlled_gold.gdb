set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/quat_norm_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $q=(char *)calloc(1,24)
  set $qd=(double *)calloc(3,8)
  set $result=(double *)calloc(1,8)
  set *((double *)$q)=0.0
  set *((int *)($q+8))=3
  set *((void **)($q+16))=$qd
  set $qd[0]=3.0
  set $qd[1]=4.0
  set $qd[2]=12.0
  set *$result=((double (*)(void *))quat_norm)((void *)$q)
  printf "QUAT_NORM_RESULT\n"
  x/1gx $result
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/quat_norm_gold.bin $result $result+1
  kill
  quit
end
run
