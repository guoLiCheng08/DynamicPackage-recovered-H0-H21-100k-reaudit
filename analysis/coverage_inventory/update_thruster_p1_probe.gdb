set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $thruster = $base + 0x217c60
set $lever = (double *)calloc(3, 8)
set $input = (double *)calloc(3, 8)
set $force = (double *)calloc(3, 8)
set $torque = (double *)calloc(3, 8)
set $lever[0] = 1.0
set $lever[1] = -2.0
set $lever[2] = 3.0
set $input[0] = 4.0
set $input[1] = -5.0
set $input[2] = 6.0
# DpVector ABI count/reserved/data for lever +0x10, input +0x38, force +0x60, torque +0x88.
set {int}($thruster + 0x10) = 3
set {void **}($thruster + 0x18) = $lever
set {int}($thruster + 0x38) = 3
set {void **}($thruster + 0x40) = $input
set {int}($thruster + 0x60) = 3
set {void **}($thruster + 0x68) = $force
set {int}($thruster + 0x88) = 3
set {void **}($thruster + 0x90) = $torque
set {double}($thruster + 0x00) = 2.5
set {int}($thruster + 0x08) = 1
call ((void (*)(void))UpdateThruster)()
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_thruster_p1_on_force.bin $force $force+3
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_thruster_p1_on_torque.bin $torque $torque+3
set $force[0] = -10.0
set $force[1] = -20.0
set $force[2] = -30.0
set $torque[0] = 11.0
set $torque[1] = 22.0
set $torque[2] = 33.0
set {int}($thruster + 0x08) = 2
call ((void (*)(void))UpdateThruster)()
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_thruster_p1_off_force.bin $force $force+3
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_thruster_p1_off_torque.bin $torque $torque+3
quit
