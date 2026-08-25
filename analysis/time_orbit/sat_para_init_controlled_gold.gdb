set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/sat_para_init_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $s=(char *)&Sat
  call (void)SatParaInit()
  printf "SAT_PARA_INIT\n"
  p/x (long)*(void **)($s+0x018)-(long)$s
  p/x (long)*(void **)($s+0x078)-(long)$s
  p/x (long)*(void **)($s+0x0d0)-(long)$s
  p/x (long)*(void **)($s+0x0f8)-(long)$s
  p/x (long)*(void **)($s+0x120)-(long)$s
  p/x (long)*(void **)($s+0x148)-(long)$s
  p/x (long)*(void **)($s+0x1b8)-(long)$s
  p/x (long)*(void **)($s+0x218)-(long)$s
  p/x (long)*(void **)($s+0x278)-(long)$s
  p/x (long)*(void **)($s+0x2d8)-(long)$s
  p/x (long)*(void **)($s+0x338)-(long)$s
  p/x (long)*(void **)($s+0x398)-(long)$s
  p/x (long)*(void **)($s+0x3f8)-(long)$s
  p/x (long)*(void **)($s+0x458)-(long)$s
  p/x (long)*(void **)($s+0x4b8)-(long)$s
  p/x (long)*(void **)($s+0x518)-(long)$s
  p/x (long)*(void **)($s+0x578)-(long)$s
  p/x (long)*(void **)($s+0x5d8)-(long)$s
  p/x (long)*(void **)($s+0x638)-(long)$s
  p/x (long)*(void **)($s+0x698)-(long)$s
  p/x (long)*(void **)($s+0x7a0)-(long)$s
  p/x (long)*(void **)($s+0x8a8)-(long)$s
  p/x (long)*(void **)($s+0x9b0)-(long)$s
  p/x (long)*(void **)($s+0xa10)-(long)$s
  p/x (long)*(void **)($s+0xd48)-(long)$s
  kill
  quit
end
run
