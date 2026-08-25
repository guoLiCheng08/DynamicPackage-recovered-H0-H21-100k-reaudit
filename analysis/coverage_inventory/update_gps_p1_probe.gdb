set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $gps = $base + 0x216900
set $flag = $base + 0x2168e0
set $time = (double *)calloc(6, 8)
set $position = (double *)calloc(3, 8)
set $velocity = (double *)calloc(3, 8)
set $time[0] = 2025.9
set $time[1] = -1.9
set $time[2] = 3.1
set $time[3] = 4.9
set $time[4] = -5.1
set $time[5] = 6.99
set $position[0] = 7000000.25
set $position[1] = -123.5
set $position[2] = 42.125
set $velocity[0] = 1.5
set $velocity[1] = -2.25
set $velocity[2] = 3.75
set {int}$flag = 1
call ((void (*)(double *, double *, double *))UpdateGPS)($time, $position, $velocity)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_gps_p1_tail.bin $gps+0x28 $gps+0x70
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_gps_p1_init_flag.bin $flag $flag+4
quit
