set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $data=(double *)calloc(12,8)
  set $vector=(void *)calloc(1,16)
  set *(int *)$vector=3
  set *(void **)($vector+8)=$data
  set $data[0]=0.0
  set $data[1]=0.0
  set $data[2]=1.0
  set $ret=(int *)calloc(4,4)
  set $ret[0]=((int (*)(void *,double))isSunInFOV)($vector,0.5)
  set $data[3]=0.0
  set $data[4]=1.0
  set $data[5]=0.0
  set *(void **)($vector+8)=$data+3
  set $ret[1]=((int (*)(void *,double))isSunInFOV)($vector,0.5)
  set $data[6]=0.0
  set $data[7]=0.0
  set $data[8]=-1.0
  set *(void **)($vector+8)=$data+6
  set $ret[2]=((int (*)(void *,double))isSunInFOV)($vector,3.2)
  set $data[9]=0.479425538604203
  set $data[10]=0.0
  set $data[11]=0.8775825618903728
  set *(void **)($vector+8)=$data+9
  set $ret[3]=((int (*)(void *,double))isSunInFOV)($vector,0.5)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_is_sun_in_fov_p3_returns.bin $ret $ret+4
  kill
  quit
end
run
