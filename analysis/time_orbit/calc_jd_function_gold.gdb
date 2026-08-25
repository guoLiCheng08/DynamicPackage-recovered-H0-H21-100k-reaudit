set pagination off
set confirm off
set print elements 64
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/calc_jd_function_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $jd_0 = ((double (*)(double, double, double, double, double, double)) Calc_JD)(2000.0, 1.0, 1.0, 12.0, 0.0, 0.0)
  set $jd_1 = ((double (*)(double, double, double, double, double, double)) Calc_JD)(2024.0, 2.0, 29.0, 6.0, 30.0, 15.5)
  set $jd_2 = ((double (*)(double, double, double, double, double, double)) Calc_JD)(1999.0, 12.0, 31.0, 23.0, 59.0, 59.25)
  set $jd_3 = ((double (*)(double, double, double, double, double, double)) Calc_JD)(1985.0, 7.0, 1.0, 0.0, 0.0, 0.125)
  printf "calc_jd case0: %.17g\n", $jd_0
  printf "calc_jd case1: %.17g\n", $jd_1
  printf "calc_jd case2: %.17g\n", $jd_2
  printf "calc_jd case3: %.17g\n", $jd_3
  kill
  quit
end
run
