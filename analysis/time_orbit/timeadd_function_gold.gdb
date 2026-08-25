set pagination off
set confirm off
set print elements 64
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/timeadd_function_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $array = (double *) malloc(48)
  call ((void (*)(double, double, double, double, double, double)) TimeInit)(2024.0, 2.0, 28.0, 23.0, 59.0, 59.75)
  call ((void (*)(double)) TimeAdd)(0.5)
  call ((void (*)(void *)) TimeArrayGet)($array)
  set $total0 = ((double (*)(void)) TimeTotalGet)()
  printf "timeadd case0 calendar\n"
  x/6fg $array
  printf "timeadd case0 total %.17g\n", $total0
  call ((void (*)(double, double, double, double, double, double)) TimeInit)(1999.0, 12.0, 31.0, 23.0, 59.0, 59.25)
  call ((void (*)(double)) TimeAdd)(2.5)
  call ((void (*)(void *)) TimeArrayGet)($array)
  set $total1 = ((double (*)(void)) TimeTotalGet)()
  printf "timeadd case1 calendar\n"
  x/6fg $array
  printf "timeadd case1 total %.17g\n", $total1
  kill
  quit
end
run
