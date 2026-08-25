set pagination off
set confirm off
set print elements 64
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/calc_precession_function_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $mat = (char *) malloc(24)
  set $data = (double *) malloc(72)
  set *(int *)($mat + 0) = 3
  set *(int *)($mat + 4) = 3
  set *(int *)($mat + 8) = 3
  set *(int *)($mat + 12) = 0
  set *(double **)($mat + 16) = $data
  call ((void (*)(double, void *)) Calc_Precession)(0.0, $mat)
  printf "precession t=0\n"
  x/9fg $data
  call ((void (*)(double, void *)) Calc_Precession)(0.2424, $mat)
  printf "precession t=0.2424\n"
  x/9fg $data
  call ((void (*)(double, void *)) Calc_Precession)(-0.5, $mat)
  printf "precession t=-0.5\n"
  x/9fg $data
  kill
  quit
end
run
