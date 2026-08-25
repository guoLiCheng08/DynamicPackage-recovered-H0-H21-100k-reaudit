set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/sada_drive_once_gold_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set *(unsigned int *)((char *)&SADA + 0x00) = 1
  set *(double *)((char *)&SADA + 0x08) = 0.3
  set *(double *)((char *)&SADA + 0x10) = -0.4
  set *(double *)((char *)&SADA + 0x18) = 0.0
  set *(double *)((char *)&SADA + 0x20) = 0.0
  set *(double *)((char *)&SADA + 0x28) = 0.0
  set *(double *)((char *)&SADA + 0x30) = 0.0
  set *(double *)((char *)&SADA + 0x38) = 0.0
  set *(double *)((char *)&SADA + 0x40) = 0.0
  set *(double *)((char *)&SADA + 0x48) = 1.0
  set *(double *)((char *)&SADA + 0x50) = 1.0
  set *(double *)((char *)&SADA + 0x58) = 0.2
  set *(double *)((char *)&SADA + 0x60) = 0.2
  call ((void (*)(double)) drive_SADA_once)(0.1)
  printf "sada_drive_once_bits\n"
  x/8gx ((char *)&SADA + 0x08)
  x/4gx ((char *)&SADA + 0x48)
  kill
  quit
end
run
