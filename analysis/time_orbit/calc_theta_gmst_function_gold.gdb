set pagination off
set confirm off
set print elements 64
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/calc_theta_gmst_function_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $theta_0 = ((double (*)(double, double, double, double, double, double)) Calc_theta_GMST)(2000.0, 1.0, 1.0, 12.0, 0.0, 0.0)
  set $theta_1 = ((double (*)(double, double, double, double, double, double)) Calc_theta_GMST)(2024.0, 2.0, 29.0, 6.0, 30.0, 15.5)
  set $theta_2 = ((double (*)(double, double, double, double, double, double)) Calc_theta_GMST)(1999.0, 12.0, 31.0, 23.0, 59.0, 59.25)
  set $theta_3 = ((double (*)(double, double, double, double, double, double)) Calc_theta_GMST)(1985.0, 7.0, 1.0, 0.0, 0.0, 0.125)
  printf "gmst case0: %.17g\n", $theta_0
  printf "gmst case1: %.17g\n", $theta_1
  printf "gmst case2: %.17g\n", $theta_2
  printf "gmst case3: %.17g\n", $theta_3
  kill
  quit
end
run
