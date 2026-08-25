set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/eccentric2mean_function_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $out0 = ((double (*)(double, double, double)) Eccentric2Mean)(2.181001813890183, 0.1, 0.81901443222271053)
  set $out1 = ((double (*)(double, double, double)) Eccentric2Mean)(-1.945363558764963, 0.65, -0.93088527760797902)
  set $out2 = ((double (*)(double, double, double)) Eccentric2Mean)(0.0, 0.9, 0.0)
  printf "eccentric2mean case0 %.17g\n", $out0
  printf "eccentric2mean case1 %.17g\n", $out1
  printf "eccentric2mean case2 %.17g\n", $out2
  kill
  quit
end
run
