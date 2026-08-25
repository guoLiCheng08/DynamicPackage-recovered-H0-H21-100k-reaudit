set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $matrix_data = (double *)calloc(6, 8)
  set $matrix_data[0] = 1.0
  set $matrix_data[1] = 2.0
  set $matrix_data[2] = 99.0
  set $matrix_data[3] = 3.0
  set $matrix_data[4] = 4.0
  set $matrix_data[5] = 99.0
  set $x_data = (double *)calloc(2, 8)
  set $x_data[0] = 2.0
  set $x_data[1] = -1.0
  set $y_data = (double *)calloc(2, 8)
  set $snapshots = (double *)calloc(8, 8)
  set $matrix = (char *)calloc(0x18, 1)
  set {int}($matrix+0x00) = 2
  set {int}($matrix+0x04) = 2
  set {int}($matrix+0x08) = 3
  set {void *}($matrix+0x10) = $matrix_data
  set $x = (char *)calloc(0x10, 1)
  set {int}($x+0x00) = 1
  set {void *}($x+0x08) = $x_data
  set $y = (char *)calloc(0x10, 1)
  set {int}($y+0x00) = 1
  set {void *}($y+0x08) = $y_data
  set $y_data[0] = 10.0
  set $y_data[1] = -3.0
  set $results = (int *)calloc(4, 4)
  set $results[0] = (int)blas_gemv($matrix, $x, $y, 2.0, 0.5)
  set $snapshots[0] = $y_data[0]
  set $snapshots[1] = $y_data[1]
  set {int}($y+0x00) = 2
  set $results[1] = (int)blas_gemv($matrix, $x, $y, 0.0, -2.0)
  set $snapshots[2] = $y_data[0]
  set $snapshots[3] = $y_data[1]
  set $results[2] = (int)blas_gemv($matrix, $x, $y, 2.0, 0.5)
  set $snapshots[4] = $y_data[0]
  set $snapshots[5] = $y_data[1]
  set {int}($matrix+0x04) = 0
  set {void *}($x+0x08) = 0
  set $y_data[0] = 8.0
  set $y_data[1] = -4.0
  set $results[3] = (int)blas_gemv($matrix, $x, $y, 2.0, 0.5)
  set $snapshots[6] = $y_data[0]
  set $snapshots[7] = $y_data[1]
  set {int}($matrix+0x00) = 0
  set {int}($y+0x00) = 0
  set $zero_row_result = (int)blas_gemv($matrix, $x, $y, 2.0, 0.5)
  set $result_blob = (char *)calloc(20, 1)
  set {int}($result_blob+0x00) = $results[0]
  set {int}($result_blob+0x04) = $results[1]
  set {int}($result_blob+0x08) = $results[2]
  set {int}($result_blob+0x0c) = $results[3]
  set {int}($result_blob+0x10) = $zero_row_result
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_blas_gemv_descriptor_nonfatal_p3_result.bin $result_blob ($result_blob+20)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_blas_gemv_descriptor_nonfatal_p3_snapshots.bin $snapshots ($snapshots+8)
  kill
  quit
end
run
