set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $q_xyz = (double *)calloc(3, 8)
  set *(unsigned long long *)($q_xyz+0) = 0xfff8000000000000
  set *(unsigned long long *)($q_xyz+1) = 0xfff8000000000000
  set *(unsigned long long *)($q_xyz+2) = 0xfff8000000000000
  set $q = (char *)calloc(1, 24)
  set *(unsigned long long *)$q = 0xfff8000000000000
  set *(int *)($q+8) = 3
  set *(void **)($q+16) = $q_xyz
  set $m_data = (double *)calloc(9,8)
  set $m = (char *)calloc(1,24)
  set *(int *)$m = 3
  set *(int *)($m+4) = 3
  set *(int *)($m+8) = 3
  set *(void **)($m+16) = $m_data
  call ((void (*)(void *, void *)) quat_att_mat)($m,$q)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_att_mat_h0_step101_neg_nan_matrix.bin $m_data ($m_data+9)
  kill
  quit
end
run
