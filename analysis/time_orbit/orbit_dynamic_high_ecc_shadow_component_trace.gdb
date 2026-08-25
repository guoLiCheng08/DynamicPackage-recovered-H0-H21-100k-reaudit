set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  call ((void (*)(double, double, double, double, double, double)) TimeInit)(2031.0, 12.0, 31.0, 23.0, 59.0, 50.0)
  set $calendar = (double *)malloc(48)
  set $calendar[0] = 2031.0
  set $calendar[1] = 12.0
  set $calendar[2] = 31.0
  set $calendar[3] = 23.0
  set $calendar[4] = 59.0
  set $calendar[5] = 50.0
  set $gci_data = (double *)malloc(24)
  set $gci_data[0] = -12000000.0
  set $gci_data[1] = 65000000.0
  set $gci_data[2] = 28000000.0
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
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_orbit_shadow_ecef_position.bin $ecef_data ($ecef_data+3)
  set $accel_data = (double *)malloc(24)
  set $accel_vec = (char *)malloc(16)
  set *(int *)$accel_vec = 3
  set *(int *)($accel_vec+4) = 0
  set *(double **)($accel_vec+8) = $accel_data
  call ((void (*)(void *,void *))orbit_dynamic)($accel_vec,$gci_vec)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_orbit_shadow_gci_accel.bin $accel_data ($accel_data+3)
  printf "ecef_bits=(%016llx,%016llx,%016llx) gci_accel_bits=(%016llx,%016llx,%016llx)\n", *(unsigned long long *)$ecef_data, *(unsigned long long *)($ecef_data+1), *(unsigned long long *)($ecef_data+2), *(unsigned long long *)$accel_data, *(unsigned long long *)($accel_data+1), *(unsigned long long *)($accel_data+2)
  kill
  quit
end
run
