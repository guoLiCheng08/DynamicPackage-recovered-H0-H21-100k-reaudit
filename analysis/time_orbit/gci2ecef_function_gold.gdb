set pagination off
set confirm off
set print elements 64
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gci2ecef_function_gold.log
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
  set $in_data[0] = 7000000.0
  set $in_data[1] = -1200000.0
  set $in_data[2] = 2500000.0
  set $time[0] = 2024.0
  set $time[1] = 2.0
  set $time[2] = 29.0
  set $time[3] = 6.0
  set $time[4] = 30.0
  set $time[5] = 15.5
  call ((void (*)(void *, void *, void *)) GCI2ECEF)($out_vec, $in_vec, $time)
  printf "gci2ecef vector case0\n"
  x/3fg $out_data
  set $in_data[0] = -1000.25
  set $in_data[1] = 4000.5
  set $in_data[2] = 5500.75
  set $time[0] = 1985.0
  set $time[1] = 7.0
  set $time[2] = 1.0
  set $time[3] = 0.0
  set $time[4] = 0.0
  set $time[5] = 0.125
  call ((void (*)(void *, void *, void *)) GCI2ECEF)($out_vec, $in_vec, $time)
  printf "gci2ecef vector case1\n"
  x/3fg $out_data
  kill
  quit
end
run
