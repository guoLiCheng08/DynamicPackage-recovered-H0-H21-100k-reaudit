set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dynamics_flex_entry_register_probe.log
set logging overwrite on
set logging enabled on
break dynamics_flex
commands
  silent
  printf "dynamics_flex entry registers\n"
  bt 4
  printf "rdi=%p rsi=%p rdx=%p rcx=%p r8=%p r9=%p\n", $rdi, $rsi, $rdx, $rcx, $r8, $r9
  printf "stack7=%p stack8=%p stack9=%p\n", *(void **)($rsp+0x940), *(void **)($rsp+0x948), *(void **)($rsp+0x950)
  x/4gx $rdi
  x/4gx $rsi
  x/4gx $rdx
  x/4gx $rcx
  x/4gx $r8
  x/4gx $r9
  kill
  quit
end
run
