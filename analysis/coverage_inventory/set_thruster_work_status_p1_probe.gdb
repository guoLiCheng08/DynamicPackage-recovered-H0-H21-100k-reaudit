set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $thruster = $base + 0x217c60
set {int}($thruster + 8) = 0
call ((void (*)(unsigned int))SetThrusterWorkStatus)(0xa5a5f00d)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_set_thruster_work_status_p1.bin $thruster+8 $thruster+12
quit
