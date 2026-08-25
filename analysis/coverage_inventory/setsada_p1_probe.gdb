set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $angle = (double *)calloc(2,8)
set $angle[0] = 1.25
set $angle[1] = -2.5
call ((void (*)(unsigned int, const double *))SetSADA)(0xa5a55a5a, $angle)
set $sada = $base + 0x216980
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_setsada_p1_prefix.bin $sada ($sada + 0x18)
quit
