set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $results = (char *)calloc(0x30, 1)
  set $lhs_data = (double *)calloc(3, 8)
  set $lhs_data[0] = 1.0
  set $lhs_data[1] = 2.0
  set $lhs_data[2] = 3.0
  set $lhs = (char *)calloc(0x10, 1)
  set {int}($lhs+0x00) = 3
  set {void *}($lhs+0x08) = $lhs_data
  set $rhs_short_data = (double *)calloc(3, 8)
  set $rhs_short_data[0] = 10.0
  set $rhs_short_data[1] = 20.0
  set $rhs_short_data[2] = 30.0
  set $rhs_short = (char *)calloc(0x10, 1)
  set {int}($rhs_short+0x00) = 2
  set {void *}($rhs_short+0x08) = $rhs_short_data
  set $dot_out = (double *)calloc(1, 8)
  set $dot_out[0] = 123.0
  set $return_code = (int)vector_add($lhs, $rhs_short)
  set {int}($results+0x00) = $return_code
  set $return_code = (int)vector_memcpy($lhs, $rhs_short)
  set {int}($results+0x04) = $return_code
  set $return_code = (int)vector_dot($lhs, $rhs_short, $dot_out)
  set {int}($results+0x08) = $return_code
  set {double}($results+0x18) = $dot_out[0]
  set $empty_left = (char *)calloc(0x10, 1)
  set $empty_right = (char *)calloc(0x10, 1)
  set $dot_out[0] = 456.0
  set $return_code = (int)vector_add($empty_left, $empty_right)
  set {int}($results+0x0c) = $return_code
  set $return_code = (int)vector_scale($empty_left, 7.0)
  set {int}($results+0x10) = $return_code
  set $return_code = (int)vector_dot($empty_left, $empty_right, $dot_out)
  set {int}($results+0x14) = $return_code
  set {double}($results+0x20) = $dot_out[0]
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_descriptor_nonfatal_p3_results.bin $results ($results+0x30)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_vector_descriptor_nonfatal_p3_lhs_data.bin $lhs_data ($lhs_data+3)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/vector_memcpy_gold.bin $results+4 $results+8
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/vector_memcpy_destination_gold.bin $lhs_data $lhs_data+3
  kill
  quit
end
run
