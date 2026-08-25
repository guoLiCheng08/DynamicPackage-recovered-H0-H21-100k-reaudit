set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $dss = $base + 0x2179a0
set $measure0 = (double *)calloc(3, 8)
set $measure1 = (double *)calloc(3, 8)
set $sun = (char *)calloc(16, 1)
set $sundata = (double *)calloc(3, 8)
set $position = (char *)calloc(16, 1)
set $positiondata = (double *)calloc(3, 8)
set $out = (int *)calloc(4, 4)
set {int}$sun = 3
set {void **}($sun + 8) = $sundata
set {int}$position = 3
set {void **}($position + 8) = $positiondata
set $positiondata[0] = 7000000.0
set $i = 0
while $i < 2
  set $item = $dss + $i * 0x158
  set {double}($item + 0x10) = 100.0
  set {int}($item + 0x108) = -77
  set {int}($item + 0x110) = 3
  if $i == 0
    set {void **}($item + 0x118) = $measure0
  else
    set {void **}($item + 0x118) = $measure1
  end
  set $i = $i + 1
end
# 同向太阳，非地影。
set $sundata[0] = 1.0
call ((void (*)(void *, void *))UpdateSunSensorValidFlag)($sun, $position)
set $out[0] = {int}($dss + 0x108)
set $out[1] = {int}($dss + 0x158 + 0x108)
# 反向太阳，地影。
set $sundata[0] = -1.0
call ((void (*)(void *, void *))UpdateSunSensorValidFlag)($sun, $position)
set $out[2] = {int}($dss + 0x108)
set $out[3] = {int}($dss + 0x158 + 0x108)
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_sun_sensor_valid_flag_p1_flags.bin $out $out+4
quit
