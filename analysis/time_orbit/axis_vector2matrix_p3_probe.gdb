set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $x=(double *)calloc(3,8)
  set $y=(double *)calloc(3,8)
  set $z=(double *)calloc(3,8)
  set $x[0]=1.0
  set $x[1]=2.0
  set $x[2]=3.0
  set $y[0]=4.0
  set $y[1]=5.0
  set $y[2]=6.0
  set $z[0]=7.0
  set $z[1]=8.0
  set $z[2]=9.0
  set $vx=(char *)calloc(0x10,1)
  set $vy=(char *)calloc(0x10,1)
  set $vz=(char *)calloc(0x10,1)
  set *(int *)$vx=3
  set *(void **)($vx+8)=$x
  set *(int *)$vy=3
  set *(void **)($vy+8)=$y
  set *(int *)$vz=3
  set *(void **)($vz+8)=$z
  set $data=(double *)calloc(9,8)
  set $out=(char *)calloc(0x18,1)
  set *(int *)$out=3
  set *(int *)($out+4)=3
  set *(int *)($out+8)=3
  set *(void **)($out+0x10)=$data
  call ((void (*)(void *,void *,void *,void *))AxisVector2Matrix)($out,$vx,$vy,$vz)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_axis_vector2matrix_p3_output.bin $data $data+9
  kill
  quit
end
run
