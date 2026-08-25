set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/sada_set_get_gold_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $command = (double *)calloc(2, 8)
  set $out = (double *)calloc(2, 8)
  set $command[0] = 0.3
  set $command[1] = -0.4
  call ((void (*)(int, void *)) SetSADA)(42, $command)
  printf "set_sada_bits\n"
  x/1wx (char *)&SADA
  x/2gx ((char *)&SADA + 8)
  set *(double *)((char *)&SADA + 0x18) = 1.25
  set *(double *)((char *)&SADA + 0x20) = -2.5
  call ((void (*)(void *)) getSADAangle)($out)
  printf "get_sada_bits\n"
  x/2gx $out
  kill
  quit
end
run
