set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $group = $base + 0x217040
set $mtq = $base + 0x217160
set $channel = (double *)calloc(6, 8)
set $out = (double *)calloc(3, 8)
set $map = (double *)calloc(18, 8)
set $i = 0
while $i < 6
  set {double}($mtq + $i * 0x38) = ($i + 1) * 1.25
  set $i = $i + 1
end
# 3×6 row-major: deliberately non-symmetric coefficients to expose order and all channels.
set $map[0] = 1.0
set $map[1] = -2.0
set $map[2] = 0.5
set $map[3] = 3.0
set $map[4] = -1.0
set $map[5] = 2.0
set $map[6] = -0.25
set $map[7] = 1.5
set $map[8] = 2.0
set $map[9] = -3.0
set $map[10] = 0.75
set $map[11] = 1.25
set $map[12] = 4.0
set $map[13] = -1.5
set $map[14] = 0.25
set $map[15] = 0.5
set $map[16] = 2.5
set $map[17] = -2.0
# DpVector ABI: count/reserved/data. DpMatrix ABI: rows/cols/stride/reserved/data.
set {int}($group + 0x00) = 3
set {void **}($group + 0x08) = $out
set {int}($group + 0x28) = 6
set {void **}($group + 0x30) = $channel
set {int}($group + 0x68) = 3
set {int}($group + 0x6c) = 6
set {int}($group + 0x70) = 6
set {void **}($group + 0x78) = $map
call ((void (*)(void))UpdateMagMoment)()
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_mag_moment_p1_channel.bin $channel ($channel + 6)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_mag_moment_p1_group.bin $out ($out + 3)
quit
