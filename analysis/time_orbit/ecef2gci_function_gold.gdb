set pagination off
set confirm off
set print elements 64
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/ecef2gci_function_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $in_vec = (char *) malloc(16)
  set $out_vec = (char *) malloc(16)
  set $in_data = (double *) malloc(24)
  set $out_data = (double *) malloc(24)
  set $time = (double *) malloc(48)
  set *(int *)($in_vec + 0) = 3
  set *(int *)($in_vec + 4) = 0
  set *(double **)($in_vec + 8) = $in_data
  set *(int *)($out_vec + 0) = 3
  set *(int *)($out_vec + 4) = 0
  set *(double **)($out_vec + 8) = $out_data
  set $in_data[0] = -548936.84341142606
  set $in_data[1] = 7075042.6046172278
  set $in_data[2] = 2516434.0811546384
  set $time[0] = 2024.0
  set $time[1] = 2.0
  set $time[2] = 29.0
  set $time[3] = 6.0
  set $time[4] = 30.0
  set $time[5] = 15.5
  call ((void (*)(void *, void *, void *)) ECEF2GCI)($out_vec, $in_vec, $time)
  printf "ecef2gci vector case0\n"
  x/3fg $out_data
  set $in_data[0] = -4107.6138144978613
  set $in_data[1] = -341.55300932089494
  set $in_data[2] = 5502.1451423758517
  set $time[0] = 1985.0
  set $time[1] = 7.0
  set $time[2] = 1.0
  set $time[3] = 0.0
  set $time[4] = 0.0
  set $time[5] = 0.125
  call ((void (*)(void *, void *, void *)) ECEF2GCI)($out_vec, $in_vec, $time)
  printf "ecef2gci vector case1\n"
  x/3fg $out_data
  kill
  quit
end
run
