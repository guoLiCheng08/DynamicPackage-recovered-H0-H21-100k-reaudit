set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $initial = (double *)calloc(1, 232)
set $initial[0] = 0.1
set $initial[7] = 2020.0
set $initial[8] = 1.0
set $initial[9] = 2.0
set $initial[10] = 3.0
set $initial[11] = 4.0
set $initial[12] = 5.0
set $initial[13] = 7000000.0
set $initial[14] = 0.001
set $initial[15] = 0.1
set $initial[16] = 0.2
set $initial[17] = 0.3
set $initial[18] = 0.4
set $initial[19] = 10.0
set $initial[23] = 11.0
set $initial[27] = 12.0
set $initial[28] = 100.0
call ((void (*)(void *))DynamicInit)($initial)
call ((void (*)(void))STS_Init)()
set $pos_data = (double *)calloc(3, 8)
set $pos_data[0] = 7000000.0
set $pos = (char *)calloc(1, 16)
set {int}$pos = 3
set {void **}($pos + 8) = $pos_data
set $sun_data = (double *)calloc(3, 8)
set $sun = (char *)calloc(1, 16)
set {int}$sun = 3
set {void **}($sun + 8) = $sun_data
set $q_xyz = (double *)calloc(3, 8)
set $q = (char *)calloc(1, 24)
set {double}$q = 1.0
set {int}($q + 8) = 3
set {void **}($q + 16) = $q_xyz
set $base = (char *)&main - 0x1340
set $sts = $base + 0x217540
set $out = (int *)calloc(12, 4)
set $dirs = (double *)calloc(12, 8)
set $dirs[0] = 1.0
set $dirs[4] = 1.0
set $dirs[8] = 1.0
set $dirs[9] = -1.0
set $case = 0
while $case < 4
  set $sun_data[0] = $dirs[$case*3]
  set $sun_data[1] = $dirs[$case*3+1]
  set $sun_data[2] = $dirs[$case*3+2]
  call ((void (*)(void *, void *, void *))Update_STS_ValidFlag)($q, $pos, $sun)
  set $out[$case*3] = {int}($sts + 0x138)
  set $out[$case*3+1] = {int}($sts + 0x2a8)
  set $out[$case*3+2] = {int}($sts + 0x418)
  set $case = $case + 1
end
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_sts_valid_flag_p1_flags.bin $out $out+12
quit
