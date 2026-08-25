set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $state = (double *)calloc(33,8)
  set *(unsigned long long *)($state+0) = 0xfff8000000000000
  set *(unsigned long long *)($state+1) = 0xfff8000000000000
  set *(unsigned long long *)($state+2) = 0xfff8000000000000
  set *(unsigned long long *)($state+3) = 0xfff8000000000000
  call ((void (*)(void *)) y_q_unit)($state)
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/time_orbit/gold_y_q_unit_h0_step101_neg_nan.bin $state ($state+4)
  kill
  quit
end
run
