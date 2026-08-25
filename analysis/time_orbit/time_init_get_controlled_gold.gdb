set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/time_init_get_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $calendar=(double *)calloc(6,8)
  set $total=(double *)calloc(1,8)
  call ((void (*)(double,double,double,double,double,double))TimeInit)(2024.0,10.0,28.0,4.0,16.0,16.25)
  call ((void (*)(void *))TimeArrayGet)((void *)$calendar)
  set *$total=((double (*)(void))TimeTotalGet)()
  printf "TIME_INIT_GET_OUTPUTS\n"
  x/6gx $calendar
  x/1gx $total
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/timeinit_gold.bin $calendar $calendar+6
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/timearrayget_gold.bin $calendar $calendar+6
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/timetotalget_gold.bin $total $total+1
  kill
  quit
end
run
