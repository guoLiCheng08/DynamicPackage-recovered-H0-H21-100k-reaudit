set pagination off
set confirm off
set print elements 64
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/timeadd_boundary_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $array = (double *) malloc(48)
  call ((void (*)(double, double, double, double, double, double)) TimeInit)(2024.0, 3.0, 1.0, 0.0, 0.0, 0.0)
  call ((void (*)(double)) TimeAdd)(-0.25)
  call ((void (*)(void *)) TimeArrayGet)($array)
  set $total0 = ((double (*)(void)) TimeTotalGet)()
  printf "timeadd boundary case0 calendar\n"
  x/6fg $array
  printf "timeadd boundary case0 total %.17g\n", $total0
  call ((void (*)(double)) TimeAdd)(1.25)
  call ((void (*)(void *)) TimeArrayGet)($array)
  set $total1 = ((double (*)(void)) TimeTotalGet)()
  printf "timeadd boundary case1 calendar\n"
  x/6fg $array
  printf "timeadd boundary case1 total %.17g\n", $total1
  kill
  quit
end
run
