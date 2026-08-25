set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  # Original ABI symbol under test: matrix_memcpy.
  set $source_data = (double *)calloc(6, 8)
  set $source_data[0] = 1.0
  set $source_data[1] = 2.0
  set $source_data[2] = 99.0
  set $source_data[3] = 3.0
  set $source_data[4] = 4.0
  set $source_data[5] = 99.0
  set $destination_data = (double *)calloc(12, 8)
  set $index = 0
  while $index < 12
    set $destination_data[$index] = -1.0
    set $index = $index + 1
  end
  set $source = (char *)calloc(0x18, 1)
  set {int}($source+0x00) = 2
  set {int}($source+0x04) = 2
  set {int}($source+0x08) = 3
  set {void *}($source+0x10) = $source_data
  set $destination = (char *)calloc(0x18, 1)
  set {int}($destination+0x00) = 3
  set {int}($destination+0x04) = 1
  set {int}($destination+0x08) = 4
  set {void *}($destination+0x10) = $destination_data
  set $return_code = (int)matrix_memcpy($destination, $source)
  set $result = (char *)calloc(8, 1)
  set {int}($result+0x00) = $return_code
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/matrix_memcpy_gold.bin $result ($result+8)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/matrix_memcpy_destination_gold.bin $destination_data ($destination_data+12)
  kill
  quit
end
run
