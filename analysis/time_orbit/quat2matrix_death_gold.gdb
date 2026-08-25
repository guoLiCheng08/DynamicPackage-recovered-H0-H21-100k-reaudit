set pagination off
set confirm off
set disable-randomization on
set unwind-on-signal on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
catch signal SIGSEGV
commands
  silent
  set {int}$death_gold = 11
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/quat2matrix_death_gold.bin $death_gold $death_gold+4
  kill
  quit
end
break main
commands
  silent
  set $death_gold=(char *)calloc(4,1)
  set $xyz=(double *)calloc(3,8)
  set $xyz[0]=-0.5
  set $xyz[1]=0.5
  set $xyz[2]=0.5
  set $quat=(char *)calloc(0x18,1)
  set *(double *)$quat=0.5
  set *(int *)($quat+8)=3
  set *(void **)($quat+0x10)=$xyz
  set $matrix_data=(double *)calloc(9,8)
  set $out=(char *)calloc(0x18,1)
  set *(int *)$out=3
  set *(int *)($out+4)=3
  set *(int *)($out+8)=3
  set *(void **)($out+0x10)=$matrix_data
  call ((void (*)(void *,void *))quat2matrix)($out,$quat)
  kill
  quit
end
run
