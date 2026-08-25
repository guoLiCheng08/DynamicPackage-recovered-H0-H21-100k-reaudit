set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/quat_conjugate_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $in=(char *)calloc(1,24)
  set $ind=(double *)calloc(3,8)
  set $out=(char *)calloc(1,24)
  set $outd=(double *)calloc(3,8)
  set *((double *)$in)=1.25
  set *((int *)($in+8))=3
  set *((void **)($in+16))=$ind
  set $ind[0]=-2.5
  set $ind[1]=0.0
  set *((unsigned long long *)($ind+2))=0x8000000000000000
  set *((int *)($out+8))=3
  set *((void **)($out+16))=$outd
  call ((void (*)(void *, void *))quat_conjugate)((void *)$out,(void *)$in)
  printf "QUAT_CONJUGATE_OUTPUT\n"
  x/1gx $out
  x/3gx $outd
  printf "QUAT_CONJUGATE_INPUT\n"
  x/1gx $in
  x/3gx $ind
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/quat_conjugate_gold_w.bin $out $out+8
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/quat_conjugate_gold_xyz.bin $outd $outd+3
  kill
  quit
end
run
