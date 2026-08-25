set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/eccentric2true_function_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $out0 = ((double (*)(double, double, double, double, double)) Eccentric2True)(2.181001813890183, 0.1, -0.57377291658888108, 0.81901443222271053, 0.99498743710662)
  set $out1 = ((double (*)(double, double, double, double, double)) Eccentric2True)(-1.945363558764963, 0.65, -0.36594409472331852, -0.93063682110172078, 0.75993420767853315)
  set $out2 = ((double (*)(double, double, double, double, double)) Eccentric2True)(0.0, 0.0, 1.0, 0.0, 1.0)
  printf "eccentric2true case0 %.17g\n", $out0
  printf "eccentric2true case1 %.17g\n", $out1
  printf "eccentric2true case2 %.17g\n", $out2
  kill
  quit
end
run
