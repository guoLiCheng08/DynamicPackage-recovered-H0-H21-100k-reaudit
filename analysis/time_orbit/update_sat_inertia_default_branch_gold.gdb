set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/update_sat_inertia_default_branch_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  call (void)SatParaInit()
  set $j=(double *)((char *)&Sat+0x20)
  set $j[0]=2.0
  set $j[1]=0.0
  set $j[2]=0.0
  set $j[3]=0.0
  set $j[4]=3.0
  set $j[5]=0.0
  set $j[6]=0.0
  set $j[7]=0.0
  set $j[8]=5.0
  call (void)Update_sat_inertia(2)
  printf "DEFAULT_SAT_J\n"
  x/9gx (char *)&Sat+0x20
  printf "DEFAULT_SAT_JINV\n"
  x/9gx (char *)&Sat+0x80
  printf "DEFAULT_GLOBAL_J\n"
  x/9gx &J_c_B_mem
  printf "DEFAULT_GLOBAL_JINV\n"
  x/9gx &J_c_B_inv_mem
  kill
  quit
end
run
