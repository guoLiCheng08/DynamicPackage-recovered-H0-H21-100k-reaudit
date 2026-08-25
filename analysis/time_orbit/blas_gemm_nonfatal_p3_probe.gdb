set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $left_data = (double *)calloc(2,8)
  set $right_data = (double *)calloc(2,8)
  set $out_data = (double *)calloc(2,8)
  set $left_data[0] = 2.0
  set $left_data[1] = 3.0
  set $right_data[0] = 4.0
  set $right_data[1] = 5.0
  set $out_data[0] = 7.0
  set $out_data[1] = 11.0
  set $left = (char *)calloc(0x18,1)
  set $right = (char *)calloc(0x18,1)
  set $out = (char *)calloc(0x18,1)
  set *(int *)$left = 1
  set *(int *)($left+4) = 2
  set *(int *)($left+8) = 2
  set *(void **)($left+0x10) = $left_data
  set *(int *)$right = 2
  set *(int *)($right+4) = 1
  set *(int *)($right+8) = 1
  set *(void **)($right+0x10) = $right_data
  set *(int *)$out = 1
  set *(int *)($out+4) = 1
  set *(int *)($out+8) = 1
  set *(void **)($out+0x10) = $out_data
  set $results = (int *)calloc(3,4)
  set $snapshots = (double *)calloc(3,8)
  set $results[0] = (int)blas_gemm($left,$right,$out,0.0,2.0)
  set $snapshots[0] = $out_data[0]
  set $results[1] = (int)blas_gemm($left,$right,$out,1.0,2.0)
  set $snapshots[1] = $out_data[0]
  set *(int *)$out = 2
  set $results[2] = (int)blas_gemm($left,$right,$out,1.0,2.0)
  set $snapshots[2] = $out_data[0]
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_blas_gemm_nonfatal_p3_results.bin $results ($results+3)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_blas_gemm_nonfatal_p3_snapshots.bin $snapshots ($snapshots+3)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_blas_gemm_nonfatal_p3_out.bin $out_data ($out_data+2)
  kill
  quit
end
run
