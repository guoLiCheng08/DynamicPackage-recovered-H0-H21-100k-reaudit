set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $sada = $base + 0x216980
set $out = (double *)calloc(6, 8)
# +0x08 command, +0x18 angle, +0x28 velocity, +0x38 acceleration, +0x48 command limit, +0x58 acceleration limit.
set {int}($sada + 0x00) = 7
set {double}($sada + 0x08) = 1.5
set {double}($sada + 0x10) = -1.25
set {double}($sada + 0x18) = 0.1
set {double}($sada + 0x20) = -0.2
set {double}($sada + 0x28) = 0.05
set {double}($sada + 0x30) = -0.10
set {double}($sada + 0x48) = 1.0
set {double}($sada + 0x50) = 1.0
set {double}($sada + 0x58) = 0.4
set {double}($sada + 0x60) = 0.6
call ((void (*)(double))drive_SADA_once)(0.25)
set $out[0] = {double}($sada + 0x18)
set $out[1] = {double}($sada + 0x20)
set $out[2] = {double}($sada + 0x28)
set $out[3] = {double}($sada + 0x30)
set $out[4] = {double}($sada + 0x38)
set $out[5] = {double}($sada + 0x40)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_drive_sada_once_p1_state.bin $out ($out + 6)
quit
