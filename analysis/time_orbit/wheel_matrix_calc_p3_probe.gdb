set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $data=(double *)calloc(12,8)
  set $out=(void *)calloc(1,24)
  set *(int *)$out=3
  set *(int *)($out+4)=4
  set *(int *)($out+8)=4
  set *(void **)($out+16)=$data
  set $c0=(double *)calloc(12,8)
  set $v0=(void *)calloc(4,16)
  set *(int *)$v0=3
  set *(void **)($v0+8)=$c0
  set *(int *)($v0+16)=3
  set *(void **)($v0+24)=$c0+3
  set *(int *)($v0+32)=3
  set *(void **)($v0+40)=$c0+6
  set *(int *)($v0+48)=3
  set *(void **)($v0+56)=$c0+9
  set $c0[0]=1.0
  set $c0[1]=2.0
  set $c0[2]=3.0
  set $c0[3]=4.0
  set $c0[4]=5.0
  set $c0[5]=6.0
  set $c0[6]=7.0
  set $c0[7]=8.0
  set $c0[8]=9.0
  set $c0[9]=10.0
  set $c0[10]=11.0
  set $c0[11]=12.0
  call (void)wheel_matrix_calc($out,$v0,$v0+16,$v0+32,$v0+48)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_wheel_matrix_calc_p3_data.bin $data $data+12
  kill
  quit
end
run
