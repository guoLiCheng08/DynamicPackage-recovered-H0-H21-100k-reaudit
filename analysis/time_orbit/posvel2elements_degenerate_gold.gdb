set pagination off
set confirm off
set print elements 64
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/posvel2elements_degenerate_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $position = (double *) malloc(24)
  set $velocity = (double *) malloc(24)
  set $o0 = (double *) malloc(64)
  call ((void (*)(void *, void *, double, double, double, double, double, double)) Elements2PosVel_nu)($position, $velocity, 7000000.0, 0.0, 0.7, 1.3, 0.45, 2.1)
  call ((void (*)(void *, void *, void *, void *, void *, void *, void *, void *, void *, void *)) PosVel2Elements_M)($o0, $o0+1, $o0+2, $o0+3, $o0+4, $o0+5, $o0+6, $o0+7, $position, $velocity)
  printf "circular-inclined position\n"
  x/3fg $position
  printf "circular-inclined velocity\n"
  x/3fg $velocity
  printf "circular-inclined elements\n"
  x/8fg $o0
  call ((void (*)(void *, void *, double, double, double, double, double, double)) Elements2PosVel_nu)($position, $velocity, 7000000.0, 0.1, 0.0, 0.0, 0.45, 2.1)
  call ((void (*)(void *, void *, void *, void *, void *, void *, void *, void *, void *, void *)) PosVel2Elements_M)($o0, $o0+1, $o0+2, $o0+3, $o0+4, $o0+5, $o0+6, $o0+7, $position, $velocity)
  printf "eccentric-equatorial position\n"
  x/3fg $position
  printf "eccentric-equatorial velocity\n"
  x/3fg $velocity
  printf "eccentric-equatorial elements\n"
  x/8fg $o0
  kill
  quit
end
run
