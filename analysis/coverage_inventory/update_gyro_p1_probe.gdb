set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $gyro = $base + 0x216a00
set $input = (char *)calloc(16, 1)
set $inputdata = (double *)calloc(3, 8)
set $out = (double *)calloc(6, 8)
set {int}$input = 3
set {void **}($input + 8) = $inputdata
set $inputdata[0] = 0.125
set $inputdata[1] = -2.5
set $inputdata[2] = 7.75
set $i = 0
while $i < 2
  set $matrix = (double *)calloc(9, 8)
  set $measure = (double *)calloc(3, 8)
  set $item = $gyro + $i * 0x320
  set $matrix[0] = 1.0
  set $matrix[4] = 1.0
  set $matrix[8] = 1.0
  set {int}($item + 0x18) = 0
  set {int}($item + 0x68) = 3
  set {int}($item + 0x6c) = 3
  set {int}($item + 0x70) = 3
  set {void **}($item + 0x78) = $matrix
  set {int}($item + 0x118) = 3
  set {void **}($item + 0x120) = $measure
  set $i = $i + 1
end
call ((void (*)(void *))UpdateGyro)($input)
set $i = 0
while $i < 2
  set $item = $gyro + $i * 0x320
  set $ptr = {double **}($item + 0x120)
  set $out[$i * 3] = $ptr[0]
  set $out[$i * 3 + 1] = $ptr[1]
  set $out[$i * 3 + 2] = $ptr[2]
  set $i = $i + 1
end
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_gyro_p1_measure.bin $out $out+6
quit
