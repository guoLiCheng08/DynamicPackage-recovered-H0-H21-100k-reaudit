set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $stream = (void *)freopen("/home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_y_q_unit_p3_stdout.bin", "w", stdout)
  set $state=(double *)calloc(33,8)
  set $i=0
  while $i < 33
    set $state[$i]=2000.0+$i
    set $i=$i+1
  end
  set $state[0]=0.001
  set $state[1]=-0.002
  set $state[2]=0.003
  set $state[3]=-0.004
  call ((void (*)(void *))y_q_unit)($state)
  call (int)fflush($stream)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_y_q_unit_smallnorm_p3_state.bin $state $state+33
  kill
  quit
end
run
