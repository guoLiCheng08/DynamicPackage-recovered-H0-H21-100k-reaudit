set pagination off
set confirm off
set print elements 64
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/elements2posvel_nu_function_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $position = (double *) malloc(24)
  set $velocity = (double *) malloc(24)
  call ((void (*)(void *, void *, double, double, double, double, double, double)) Elements2PosVel_nu)($position, $velocity, 7000000.0, 0.1, 0.7, 1.3, 0.45, 2.2616708994930574)
  printf "elements2posvel_nu case0 position\n"
  x/3fg $position
  printf "elements2posvel_nu case0 velocity\n"
  x/3fg $velocity
  call ((void (*)(void *, void *, double, double, double, double, double, double)) Elements2PosVel_nu)($position, $velocity, 42164000.0, 0.65, 0.2, -0.8, 2.4, 3.5920276336741965)
  printf "elements2posvel_nu case1 position\n"
  x/3fg $position
  printf "elements2posvel_nu case1 velocity\n"
  x/3fg $velocity
  kill
  quit
end
run
