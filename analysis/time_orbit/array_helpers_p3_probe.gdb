set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $source = (double *)calloc(33,8)
  set $copy_destination = (double *)calloc(33,8)
  set $sum_destination = (double *)calloc(33,8)
  set $i = 0
  while $i < 33
    set $source[$i] = $i + 0.25
    set $copy_destination[$i] = 100.0 - $i
    set $sum_destination[$i] = 100.0 - $i
    set $i = $i + 1
  end
  set *(unsigned long long *)((char *)$source+8) = 0x8000000000000000
  call ((void (*)(void *,void *))array_copy)($copy_destination,$source)
  call ((void (*)(void *,void *,double))array_sum)($source,$sum_destination,0.5)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_array_copy_p3_output.bin $copy_destination $copy_destination+33
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_array_sum_p3_output.bin $sum_destination $sum_destination+33
  kill
  quit
end
run
