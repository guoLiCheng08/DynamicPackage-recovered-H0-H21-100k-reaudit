set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $in = (double *)calloc(3, 8)
set $out = (double *)calloc(3, 8)
set $in[0] = 1.25
set $in[1] = -2.5
set $in[2] = 3.75
set $out[0] = 9.0
set $out[1] = 8.0
set $out[2] = 7.0
# vector ABI [count,reserved,data]
set $vin = (char *)calloc(16, 1)
set $vout = (char *)calloc(16, 1)
set {int}$vin = 3
set {void **}($vin + 8) = $in
set {int}$vout = 3
set {void **}($vout + 8) = $out
call ((int (*)(void *, void *))RWheelTorque)($vout, $vin)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_rwheel_torque_p1_out.bin $out ($out + 3)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_rwheel_torque_p1_in.bin $in ($in + 3)
quit
