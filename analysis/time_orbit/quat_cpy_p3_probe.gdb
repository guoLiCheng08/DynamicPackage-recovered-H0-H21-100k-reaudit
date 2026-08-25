set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $source = (double *)calloc(4,8)
  set $destination = (double *)calloc(4,8)
  set $source[0] = 1.25
  set *(unsigned long long *)((char *)$source+8) = 0x8000000000000000
  set $source[2] = 3.0
  set $source[3] = -4.5
  set $destination[0] = 99.0
  set $destination[1] = 98.0
  set $destination[2] = 97.0
  set $destination[3] = 96.0
  call ((void (*)(void *,void *))quat_cpy)($destination,$source)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_quat_cpy_p3_output.bin $destination $destination+4
  kill
  quit
end
run
