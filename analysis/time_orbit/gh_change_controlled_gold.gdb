set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gh_change_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $g=(double *)calloc(196,8)
  set $h=(double *)calloc(196,8)
  call ((void (*)(void *, void *, double))gh_change)((void *)$g, (void *)$h, 2027.25)
  printf "GH_CHANGE_G_FIRST_LAST\n"
  x/4gx $g
  x/4gx $g+192
  printf "GH_CHANGE_H_FIRST_LAST\n"
  x/4gx $h
  x/4gx $h+192
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gh_change_gold_g.bin $g $g+196
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gh_change_gold_h.bin $h $h+196
  kill
  quit
end
run
