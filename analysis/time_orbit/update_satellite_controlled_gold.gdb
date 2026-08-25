set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/update_satellite_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  call (void)SatParaInit()
  call (void)UpdateSatellite()
  printf "UPDATE_SATELLITE_SAT_PREFIX_SUFFIX\n"
  x/4gx &Sat
  x/4gx (char *)&Sat+0x1058
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/update_satellite_gold_sat.bin &Sat (char *)&Sat+0x1078
  kill
  quit
end
run
