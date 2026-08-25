set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $out=(double *)calloc(4,8)
  set $out[0]=((double (*)(double,double,double,double,double))Eccentric2True)(0.1,0.2,1.0,1.0,0.5)
  set $out[1]=((double (*)(double,double,double,double,double))Eccentric2True)(-1.0,0.5,-1.0,1.0,0.2)
  set $out[2]=((double (*)(double,double,double,double,double))Eccentric2True)(0.0,0.0,0.0,1.0,1.0)
  set $out[3]=((double (*)(double,double,double,double,double))Eccentric2True)(3.0,0.1,0.0,-1.0,0.9)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_eccentric2true_p3_outputs.bin $out $out+4
  kill
  quit
end
run
