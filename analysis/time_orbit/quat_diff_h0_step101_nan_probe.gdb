set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $q_xyz = (double *)calloc(3, 8)
  set $omega_data = (double *)calloc(3, 8)
  set $out_xyz = (double *)calloc(3, 8)
  set *(unsigned long long *)($q_xyz+0) = 0xfff8000000000000
  set *(unsigned long long *)($q_xyz+1) = 0xfff8000000000000
  set *(unsigned long long *)($q_xyz+2) = 0xfff8000000000000
  set *(unsigned long long *)($omega_data+0) = 0xfff8000000000000
  set *(unsigned long long *)($omega_data+1) = 0xfff8000000000000
  set *(unsigned long long *)($omega_data+2) = 0xfff8000000000000
  set $q = (char *)calloc(1, 24)
  set $out = (char *)calloc(1, 24)
  set $omega = (char *)calloc(1, 16)
  set *(unsigned long long *)$q = 0xfff8000000000000
  set *(int *)($q+8) = 3
  set *(void **)($q+16) = $q_xyz
  set *(int *)($out+8) = 3
  set *(void **)($out+16) = $out_xyz
  set *(int *)$omega = 3
  set *(void **)($omega+8) = $omega_data
  call ((void (*)(void *, void *, void *)) quat_diff)($out,$q,$omega)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_diff_h0_step101_neg_nan.bin $out ($out+8)
  append binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_diff_h0_step101_neg_nan.bin $out_xyz ($out_xyz+3)
  kill
  quit
end
run
