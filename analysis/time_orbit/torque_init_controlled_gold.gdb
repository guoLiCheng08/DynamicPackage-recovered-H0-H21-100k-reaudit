set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/torque_init_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $t=(char *)&SatTorque
  call (void)TorqueInit()
  printf "TORQUE_INIT\n"
  p/x (long)*(void **)($t+0x08)-(long)$t
  p/x (long)*(void **)($t+0x18)-(long)$t
  p/x (long)*(void **)($t+0x28)-(long)$t
  p/x (long)*(void **)($t+0x38)-(long)$t
  p/x (long)*(void **)($t+0x48)-(long)$t
  p/x (long)*(void **)($t+0x58)-(long)$t
  p/x (long)*(void **)($t+0x68)-(long)$t
  p/x (long)*(void **)($t+0x78)-(long)$t
  p/x (long)*(void **)($t+0x148)-(long)$t
  kill
  quit
end
run
