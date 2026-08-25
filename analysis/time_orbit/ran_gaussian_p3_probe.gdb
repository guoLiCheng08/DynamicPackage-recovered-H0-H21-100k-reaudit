set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  # Original ABI symbols under test: ran_gaussian and ran_gaussian2.
  call srand(1)
  set $out=(double *)calloc(4,8)
  set $out[0]=((double (*)(double))ran_gaussian)(1.0)
  set $out[1]=((double (*)(double))ran_gaussian)(2.0)
  call srand(1)
  set $out[2]=((double (*)(double,double))ran_gaussian2)(3.0,1.0)
  set $out[3]=((double (*)(double,double))ran_gaussian2)(-2.0,0.5)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_ran_gaussian_p3_outputs.bin $out $out+4
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/ran_gaussian2_gold.bin $out+2 $out+4
  kill
  quit
end
run
