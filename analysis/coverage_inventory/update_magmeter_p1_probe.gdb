set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
break main
run
set $base = (char *)&main - 0x1340
set $meter = $base + 0x2172c0
set $input = (char *)calloc(16, 1)
set $inputdata = (double *)calloc(3, 8)
set $out = (double *)calloc(6, 8)
set {int}$input = 3
set {void **}($input + 8) = $inputdata
set $inputdata[0] = -0.875
set $inputdata[1] = 4.5
set $inputdata[2] = 0.0625
set $i = 0
while $i < 2
  set $matrix = (double *)calloc(9, 8)
  set $measure = (double *)calloc(3, 8)
  set $item = $meter + $i * 0x138
  set $matrix[0] = 1.0
  set $matrix[4] = 1.0
  set $matrix[8] = 1.0
  set {int}($item + 0x10) = 0
  set {int}($item + 0x60) = 3
  set {int}($item + 0x64) = 3
  set {int}($item + 0x68) = 3
  set {void **}($item + 0x70) = $matrix
  set {int}($item + 0x110) = 3
  set {void **}($item + 0x118) = $measure
  set $i = $i + 1
end
call ((void (*)(void *))UpdateMagMeter)($input)
set $i = 0
while $i < 2
  set $item = $meter + $i * 0x138
  set $ptr = {double **}($item + 0x118)
  set $out[$i * 3] = $ptr[0]
  set $out[$i * 3 + 1] = $ptr[1]
  set $out[$i * 3 + 2] = $ptr[2]
  set $i = $i + 1
end
dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/coverage_inventory/gold_update_magmeter_p1_measure.bin $out $out+6
quit
