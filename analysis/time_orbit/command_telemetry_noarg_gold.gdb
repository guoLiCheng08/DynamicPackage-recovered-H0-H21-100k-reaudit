set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  # Original ABI symbols under test: Algorithm_Command_Execute, DYN_Command_Execute,
  # Set_Algorithm_Telemetry_Frame_1, and Set_Algorithm_Telemetry_Frame_2.
  set $algorithm_sentinel=(unsigned char *)calloc(1,1)
  set *$algorithm_sentinel=0xa5
  call (void)Algorithm_Command_Execute()
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/algorithm_command_execute_gold.bin $algorithm_sentinel $algorithm_sentinel+1
  set $dyn_sentinel=(unsigned char *)calloc(1,1)
  set *$dyn_sentinel=0x5a
  call (void)DYN_Command_Execute()
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dyn_command_execute_gold.bin $dyn_sentinel $dyn_sentinel+1
  set $telemetry_1=(int *)calloc(1,4)
  set *$telemetry_1=(int)Set_Algorithm_Telemetry_Frame_1()
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/set_algorithm_telemetry_frame_1_gold.bin $telemetry_1 $telemetry_1+1
  set $telemetry_2=(int *)calloc(1,4)
  set *$telemetry_2=(int)Set_Algorithm_Telemetry_Frame_2()
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/set_algorithm_telemetry_frame_2_gold.bin $telemetry_2 $telemetry_2+1
  kill
  quit
end
run
