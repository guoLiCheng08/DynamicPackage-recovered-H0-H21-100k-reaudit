set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $out=(double *)calloc(8,8)
  set $out[0]=((double (*)(double,double))NiceAngle)(1.0,1.0)
  set $out[1]=((double (*)(double,double))NiceAngle)(1.0,-1.0)
  set $out[2]=((double (*)(double,double))NiceAngle)(-1.0,1.0)
  set $out[3]=((double (*)(double,double))NiceAngle)(-1.0,-1.0)
  set $out[4]=((double (*)(double,double))NiceAngle)(0.0,1.0)
  set $out[5]=((double (*)(double,double))NiceAngle)(0.0,-1.0)
  set $out[6]=((double (*)(double,double))NiceAngle)(1.0,0.0)
  set $out[7]=((double (*)(double,double))NiceAngle)(0.0,0.0)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_niceangle_p3_outputs.bin $out $out+8
  kill
  quit
end
run
