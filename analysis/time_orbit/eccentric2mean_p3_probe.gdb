set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $out=(double *)calloc(4,8)
  set $out[0]=((double (*)(double,double,double))Eccentric2Mean)(1.0,0.2,0.5)
  set $out[1]=((double (*)(double,double,double))Eccentric2Mean)(-2.0,0.7,-0.3)
  set $out[2]=((double (*)(double,double,double))Eccentric2Mean)(6.0,0.0,-1.0)
  set $out[3]=((double (*)(double,double,double))Eccentric2Mean)(0.1,0.9,0.09983341664682815)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_eccentric2mean_p3_outputs.bin $out $out+4
  kill
  quit
end
run
