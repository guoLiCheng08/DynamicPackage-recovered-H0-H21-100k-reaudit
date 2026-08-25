set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $p=(double *)calloc(3,8)
  set $v=(double *)calloc(9,8)
  set $pos=(void *)calloc(1,16)
  set $view=(void *)calloc(1,16)
  set *(int *)$pos=3
  set *(void **)($pos+8)=$p
  set *(int *)$view=3
  set *(void **)($view+8)=$v
  set $p[0]=7000000.0
  set $v[0]=-1.0
  set $ret=(int *)calloc(3,4)
  set $ret[0]=((int (*)(void *,void *,double))isEarthOut)($pos,$view,0.0)
  set $v[3]=0.0
  set $v[4]=1.0
  set $v[5]=0.0
  set *(void **)($view+8)=$v+3
  set $ret[1]=((int (*)(void *,void *,double))isEarthOut)($pos,$view,0.0)
  set $v[6]=1.0
  set $v[7]=0.0
  set $v[8]=0.0
  set *(void **)($view+8)=$v+6
  set $ret[2]=((int (*)(void *,void *,double))isEarthOut)($pos,$view,0.0)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_is_earth_out_p3_returns.bin $ret $ret+3
  kill
  quit
end
run
