set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run

# algorithm_telemetry_size has original image VMA 0x218004. Compute through main
# to remain correct for PIE execution.
set $base = (char *)&main - 0x1340
set $size = (int *)($base + 0x218004)
set *$size = 0x12345678
call ((void (*)(void))Update_Telemetry_Frame_1)()
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_telemetry/gold_update_telemetry_frame_1_size.bin $size ($size + 1)
set *$size = 0x7f7f7f7f
call ((void (*)(void))Update_Telemetry_Frame_2)()
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_telemetry/gold_update_telemetry_frame_2_size.bin $size ($size + 1)
set $retbuf = (int *)calloc(1, 4)
set *$size = 0x2468ace0
set *$retbuf = ((int (*)(void))Set_Algorithm_Telemetry_Frame_1)()
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_telemetry/gold_set_telemetry_frame_1_size.bin $size ($size + 1)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_telemetry/gold_set_telemetry_frame_1_ret.bin $retbuf ($retbuf + 1)
set *$size = 0x13579bdf
set *$retbuf = ((int (*)(void))Set_Algorithm_Telemetry_Frame_2)()
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_telemetry/gold_set_telemetry_frame_2_size.bin $size ($size + 1)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/p0_telemetry/gold_set_telemetry_frame_2_ret.bin $retbuf ($retbuf + 1)
quit
