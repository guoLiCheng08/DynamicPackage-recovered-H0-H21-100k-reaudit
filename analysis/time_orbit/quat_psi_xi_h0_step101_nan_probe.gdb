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
  set $psi_data = (double *)calloc(12,8)
  set $xi_data = (double *)calloc(12,8)
  set $psi = (char *)calloc(1,24)
  set $xi = (char *)calloc(1,24)
  set *(int *)$psi = 4
  set *(int *)($psi+4) = 3
  set *(int *)($psi+8) = 3
  set *(void **)($psi+16) = $psi_data
  set *(int *)$xi = 4
  set *(int *)($xi+4) = 3
  set *(int *)($xi+8) = 3
  set *(void **)($xi+16) = $xi_data
  call ((void (*)(void *, void *)) quat_psi)($psi,$q)
  call ((void (*)(void *, void *)) quat_xi)($xi,$q)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_psi_h0_step101_neg_nan.bin $psi_data ($psi_data+12)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_xi_h0_step101_neg_nan.bin $xi_data ($xi_data+12)
  kill
  quit
end
run
