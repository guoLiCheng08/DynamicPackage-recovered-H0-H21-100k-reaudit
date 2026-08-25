set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $calendar = (double *)malloc(48)
  set $calendar[0] = 2031.0
  set $calendar[1] = 12.0
  set $calendar[2] = 31.0
  set $calendar[3] = 23.0
  set $calendar[4] = 59.0
  set $calendar[5] = 50.0
  set $gci_data = (double *)malloc(24)
  set *(unsigned long long *)($gci_data+0) = 0x3f8a7e04672bb1bd
  set *(unsigned long long *)($gci_data+1) = 0xbfb1f019465fb335
  set *(unsigned long long *)($gci_data+2) = 0xbf9f02303c5d0ffb
  set $gci_vec = (char *)malloc(16)
  set *(int *)$gci_vec = 3
  set *(int *)($gci_vec+4) = 0
  set *(double **)($gci_vec+8) = $gci_data
  set $ecef_data = (double *)malloc(24)
  set $ecef_vec = (char *)malloc(16)
  set *(int *)$ecef_vec = 3
  set *(int *)($ecef_vec+4) = 0
  set *(double **)($ecef_vec+8) = $ecef_data
  call ((void (*)(void *,void *,void *))GCI2ECEF)($ecef_vec,$gci_vec,$calendar)
  printf "ecef_bits=(%016llx,%016llx,%016llx)\n", *(unsigned long long *)$ecef_data, *(unsigned long long *)($ecef_data+1), *(unsigned long long *)($ecef_data+2)
  kill
  quit
end
run
