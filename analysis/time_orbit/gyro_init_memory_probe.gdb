set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gyro_init_memory_probe.log
set logging overwrite on
set logging enabled on
break *Gyro_Init
commands
  silent
  printf "entry Gyro=%p\n", &Gyro
  x/100gx &Gyro
  tbreak *(Gyro_Init+0x68)
  continue
end
break *(Gyro_Init+0x68)
commands
  silent
  printf "post Gyro=%p\n", &Gyro
  x/100gx &Gyro
  kill
  quit
end
run
