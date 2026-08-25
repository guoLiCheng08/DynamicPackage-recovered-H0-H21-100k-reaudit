set pagination off
set confirm off
set print elements 64
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/elements2posvel_function_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $position = (double *) malloc(24)
  set $velocity = (double *) malloc(24)
  call ((void (*)(void *, void *, double, double, double, double, double, double)) Elements2PosVel_M)($position, $velocity, 7000000.0, 0.1, 0.7, 1.3, 0.45, 2.1)
  printf "elements2posvel case0 position\n"
  x/3fg $position
  printf "elements2posvel case0 velocity\n"
  x/3fg $velocity
  call ((void (*)(void *, void *, double, double, double, double, double, double)) Elements2PosVel_M)($position, $velocity, 42164000.0, 0.65, 0.2, -0.8, 2.4, -1.7)
  printf "elements2posvel case1 position\n"
  x/3fg $position
  printf "elements2posvel case1 velocity\n"
  x/3fg $velocity
  set $out=(double *)calloc(12,8)
  call ((void (*)(void *, void *, double, double, double, double, double, double)) Elements2PosVel_M)($out, $out+3, 7000000.0, 0.1, 0.7, 1.3, 0.45, 2.1)
  call ((void (*)(void *, void *, double, double, double, double, double, double)) Elements2PosVel_M)($out+6, $out+9, 42164000.0, 0.65, 0.2, -0.8, 2.4, -1.7)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/elements2posvel_m_gold.bin $out $out+12
  kill
  quit
end
run
