set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $sada = $base + 0x216980
set $out = (double *)calloc(2, 8)
set $out[0] = 123.0
set $out[1] = -456.0
set {double}($sada + 0x18) = -1.23456789012345
set {double}($sada + 0x20) = 9.87654321098765
call ((void (*)(double *))getSADAangle)($out)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_get_sada_angle_p1_out.bin $out ($out + 2)
quit
