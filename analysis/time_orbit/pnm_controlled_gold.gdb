set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/pnm_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $p=(double *)calloc(196,8)
  set $d=(double *)calloc(196,8)
  call ((void (*)(void *, void *, double))Pnm)((void *)$p, (void *)$d, 1.1)
  printf "PNM_P_FIRST_LAST\n"
  x/4gx $p
  x/4gx $p+192
  printf "PNM_D_FIRST_LAST\n"
  x/4gx $d
  x/4gx $d+192
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/pnm_gold_p.bin $p $p+196
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/pnm_gold_d.bin $d $d+196
  kill
  quit
end
run
