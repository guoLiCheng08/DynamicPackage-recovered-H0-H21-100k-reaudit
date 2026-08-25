set pagination off
set confirm off
set print elements 64
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/m2e_function_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  # Original ABI symbol under test: M2E.
  set $m2e_0 = ((double (*)(double, double)) M2E)(0.0, 0.0)
  set $m2e_1 = ((double (*)(double, double)) M2E)(1.0, 0.1)
  set $m2e_2 = ((double (*)(double, double)) M2E)(2.2, 0.65)
  set $m2e_3 = ((double (*)(double, double)) M2E)(-1.7, 0.25)
  printf "m2e case0 M=0 e=0: %.17g\n", $m2e_0
  printf "m2e case1 M=1 e=0.1: %.17g\n", $m2e_1
  printf "m2e case2 M=2.2 e=0.65: %.17g\n", $m2e_2
  printf "m2e case3 M=-1.7 e=0.25: %.17g\n", $m2e_3
  set $m2e_values=(double *)calloc(4,8)
  set $m2e_values[0]=$m2e_0
  set $m2e_values[1]=$m2e_1
  set $m2e_values[2]=$m2e_2
  set $m2e_values[3]=$m2e_3
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/m2e_gold.bin $m2e_values $m2e_values+4
  kill
  quit
end
run
