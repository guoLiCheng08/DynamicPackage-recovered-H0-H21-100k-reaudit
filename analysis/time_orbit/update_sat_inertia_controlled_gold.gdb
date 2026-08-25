set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/update_sat_inertia_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  call (void)SatParaInit()
  call (void)Update_sat_inertia(0)
  printf "FLAG0_SAT_J\n"
  x/9gx (char *)&Sat+0x20
  printf "FLAG0_SAT_JINV\n"
  x/9gx (char *)&Sat+0x80
  printf "FLAG0_GLOBAL_J\n"
  x/9gx &J_c_B_mem
  printf "FLAG0_GLOBAL_JINV\n"
  x/9gx &J_c_B_inv_mem
  call (void)Update_sat_inertia(1)
  printf "FLAG1_SAT_J\n"
  x/9gx (char *)&Sat+0x20
  printf "FLAG1_SAT_JINV\n"
  x/9gx (char *)&Sat+0x80
  printf "FLAG1_GLOBAL_J\n"
  x/9gx &J_c_B_mem
  printf "FLAG1_GLOBAL_JINV\n"
  x/9gx &J_c_B_inv_mem
  kill
  quit
end
run
