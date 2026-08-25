set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/quat_att_mat_gold_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $q_xyz = (double *)calloc(3, 8)
  set $q_xyz[0] = 0.1
  set $q_xyz[1] = -0.2
  set $q_xyz[2] = 0.3
  set $q = (char *)calloc(1, 24)
  set *(double *)$q = 1.0
  set *(int *)($q+8) = 3
  set *(void **)($q+16) = $q_xyz
  set $m_data = (double *)calloc(9,8)
  set $m = (char *)calloc(1,24)
  set *(int *)$m = 3
  set *(int *)($m+4) = 3
  set *(int *)($m+8) = 3
  set *(void **)($m+16) = $m_data
  call ((void (*)(void *, void *)) quat_att_mat)($m,$q)
  printf "quat_att_mat_bits\n"
  x/9gx $m_data
  kill
  quit
end
run
