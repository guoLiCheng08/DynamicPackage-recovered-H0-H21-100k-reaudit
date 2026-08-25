set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $a=(double *)calloc(3,8)
  set $b=(double *)calloc(9,8)
  set $va=(void *)calloc(1,16)
  set $vb=(void *)calloc(1,16)
  set *(int *)$va=3
  set *(int *)$vb=3
  set *(void **)($va+8)=$a
  set *(void **)($vb+8)=$b
  set $a[2]=1.0
  set $b[2]=1.0
  set $out=(int *)calloc(3,4)
  set $out[0]=((int (*)(void *,void *,double))isSunOut)($va,$vb,0.0)
  set $b[3]=0.0
  set $b[4]=1.0
  set $b[5]=0.0
  set *(void **)($vb+8)=$b+3
  set $out[1]=((int (*)(void *,void *,double))isSunOut)($va,$vb,0.5)
  set $b[6]=0.479425538604203
  set $b[7]=0.0
  set $b[8]=0.8775825618903728
  set *(void **)($vb+8)=$b+6
  set $out[2]=((int (*)(void *,void *,double))isSunOut)($va,$vb,0.5)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_is_sun_out_p3_returns.bin $out $out+3
  kill
  quit
end
run
