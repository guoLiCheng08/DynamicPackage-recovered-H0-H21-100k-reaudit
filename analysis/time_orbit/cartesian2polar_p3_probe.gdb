set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $data=(double *)calloc(3,8)
  set $data[0]=3.0
  set $data[1]=4.0
  set $data[2]=12.0
  set $vec=(char *)calloc(0x10,1)
  set *(int *)$vec=3
  set *(void **)($vec+8)=$data
  set $out=(double *)calloc(3,8)
  call ((void (*)(void *,void *,void *,void *))Cartesian2Polar)($out,$out+1,$out+2,$vec)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_cartesian2polar_p3_output.bin $out $out+3
  kill
  quit
end
run
