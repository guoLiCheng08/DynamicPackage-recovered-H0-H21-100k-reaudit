set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $source = (unsigned char *)calloc(8,1)
  set $destination = (unsigned char *)calloc(16,1)
  set $i = 0
  while $i < 8
    set $source[$i] = 0x10 + $i
    set $destination[$i] = 0xa5
    set $destination[$i+8] = 0x5a
    set $i = $i + 1
  end
  call ((void (*)(void *,void *,int))Change_Endian)($destination,$source,8)
  call ((void (*)(void *,void *,int))Change_Endian)($destination+8,$source,0)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_change_endian_p3_output.bin $destination $destination+16
  kill
  quit
end
run
