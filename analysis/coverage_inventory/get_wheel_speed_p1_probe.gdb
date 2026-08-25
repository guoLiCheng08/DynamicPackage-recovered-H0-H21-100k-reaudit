set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $wheel = $base + 0x217e00 + 2 * 0x78
set {int}($wheel + 4) = 0
set {double}($wheel + 0x30) = -12.75
set $retbuf = (double *)calloc(1, 8)
set *$retbuf = ((double (*)(int))GetWheelSpeed)(2)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_get_wheel_speed_p1_no_noise.bin $retbuf ($retbuf + 1)
quit
