set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $sun_data = (double *)calloc(3, 8)
set $pos_data = (double *)calloc(3, 8)
set $sun = (char *)calloc(16, 1)
set $pos = (char *)calloc(16, 1)
set $out = (int *)calloc(2, 4)
set {int}$sun = 3
set {void **}($sun + 8) = $sun_data
set {int}$pos = 3
set {void **}($pos + 8) = $pos_data
set $pos_data[0] = 7000000.0
set $pos_data[1] = 0.0
set $pos_data[2] = 0.0
# 同向：separation=0，非阴影。
set $sun_data[0] = 1.0
set $out[0] = ((int (*)(void *, void *))isEarthShadow)($sun, $pos)
# 反向：separation=pi，进入阴影。
set $sun_data[0] = -1.0
set $out[1] = ((int (*)(void *, void *))isEarthShadow)($sun, $pos)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_is_earth_shadow_p1_returns.bin $out $out+2
quit
