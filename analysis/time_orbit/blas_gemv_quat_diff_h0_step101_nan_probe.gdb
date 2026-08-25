set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $q_xyz = (double *)calloc(3, 8)
  set $omega_data = (double *)calloc(3, 8)
  set $xi_data = (double *)calloc(12,8)
  set $derivative_data = (double *)calloc(4,8)
  set *(unsigned long long *)($q_xyz+0) = 0xfff8000000000000
  set *(unsigned long long *)($q_xyz+1) = 0xfff8000000000000
  set *(unsigned long long *)($q_xyz+2) = 0xfff8000000000000
  set *(unsigned long long *)($omega_data+0) = 0xfff8000000000000
  set *(unsigned long long *)($omega_data+1) = 0xfff8000000000000
  set *(unsigned long long *)($omega_data+2) = 0xfff8000000000000
  set $q = (char *)calloc(1,24)
  set $xi = (char *)calloc(1,24)
  set $omega = (char *)calloc(1,16)
  set $derivative = (char *)calloc(1,16)
  set *(unsigned long long *)$q = 0xfff8000000000000
  set *(int *)($q+8) = 3
  set *(void **)($q+16) = $q_xyz
  set *(int *)$xi = 4
  set *(int *)($xi+4) = 3
  set *(int *)($xi+8) = 3
  set *(void **)($xi+16) = $xi_data
  set *(int *)$omega = 3
  set *(void **)($omega+8) = $omega_data
  set *(int *)$derivative = 4
  set *(void **)($derivative+8) = $derivative_data
  call ((void (*)(void *,void *)) quat_xi)($xi,$q)
  call ((int (*)(void *,void *,void *,double,double)) blas_gemv)($xi,$omega,$derivative,0.5,0.0)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_blas_gemv_quat_diff_h0_step101_neg_nan.bin $derivative_data ($derivative_data+4)
  kill
  quit
end
run
