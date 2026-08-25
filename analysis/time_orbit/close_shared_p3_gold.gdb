set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  # Original ABI symbol under test: close_shared.
  call (void)close_shared()
  call (int)init_shared()
  set $result=(char *)calloc(16,1)
  call (void)close_shared()
  set *(void **)($result+0)=*(void **)&g_data
  set *(int *)($result+8)=*(int *)&g_shm_fd
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/close_shared_gold.bin $result $result+12
  kill
  quit
end
run
