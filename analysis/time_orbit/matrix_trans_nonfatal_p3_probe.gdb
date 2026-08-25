set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $src_data = (double *)calloc(6,8)
  set $dst_data = (double *)calloc(12,8)
  set $src_data[0] = 1.0
  set $src_data[1] = 2.0
  set $src_data[2] = 99.0
  set $src_data[3] = 3.0
  set $src_data[4] = 4.0
  set $src_data[5] = 99.0
  set $index = 0
  while $index < 12
    set $dst_data[$index] = -1.0
    set $index = $index + 1
  end
  set $src = (char *)calloc(0x18,1)
  set $dst = (char *)calloc(0x18,1)
  set *(int *)$src = 2
  set *(int *)($src+4) = 2
  set *(int *)($src+8) = 3
  set *(void **)($src+0x10) = $src_data
  set *(int *)$dst = 1
  set *(int *)($dst+4) = 3
  set *(int *)($dst+8) = 4
  set *(void **)($dst+0x10) = $dst_data
  set $result = (int)matrix_trans($dst,$src)
  set $result_blob = (int *)calloc(1,4)
  set $result_blob[0] = $result
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_matrix_trans_nonfatal_p3_result.bin $result_blob ($result_blob+1)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_matrix_trans_nonfatal_p3_dst.bin $dst_data ($dst_data+12)
  kill
  quit
end
run
