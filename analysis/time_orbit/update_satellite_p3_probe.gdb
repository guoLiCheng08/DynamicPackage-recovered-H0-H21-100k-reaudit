set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $base=(long)$pc
  set $yp=(double *)($base+0x2171a0)
  set $tp=(double *)($base+0x217180)
  set $sp=(double *)($base+0x214258)
  set $yp[0]=1.0
  set $yp[32]=33.0
  set *$tp=123.25
  set *$sp=-0.125
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_update_satellite_p3_before.bin $yp $yp+35
  call (void)UpdateSatellite()
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_update_satellite_p3_after.bin $yp $yp+35
  kill
  quit
end
run
