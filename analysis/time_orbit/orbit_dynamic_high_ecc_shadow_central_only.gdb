set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  call ((void (*)(double, double, double, double, double, double)) TimeInit)(2031.0, 12.0, 31.0, 23.0, 59.0, 50.0)
  printf "before J=(%.17g,%.17g,%.17g)\n", *(double *)&J2, *(double *)&J3, *(double *)&J4
  set {double}&J2 = 0.0
  set {double}&J3 = 0.0
  set {double}&J4 = 0.0
  printf "after J=(%.17g,%.17g,%.17g)\n", *(double *)&J2, *(double *)&J3, *(double *)&J4
  set $position_data = (double *)malloc(24)
  set $position_data[0] = -12000000.0
  set $position_data[1] = 65000000.0
  set $position_data[2] = 28000000.0
  set $position_vector = (char *)malloc(16)
  set *(int *)$position_vector = 3
  set *(int *)($position_vector+4) = 0
  set *(double **)($position_vector+8) = $position_data
  set $output_data = (double *)malloc(24)
  set $output_vector = (char *)malloc(16)
  set *(int *)$output_vector = 3
  set *(int *)($output_vector+4) = 0
  set *(double **)($output_vector+8) = $output_data
  set $force_data = *(double **)((char *)&F_I_external + 8)
  set $force_data[0] = 0.0
  set $force_data[1] = 0.0
  set $force_data[2] = 0.0
  set *(double *)&SpacecraftMass = 1000.0
  call ((void (*)(void *, void *)) orbit_dynamic)($output_vector, $position_vector)
  printf "central_bits=(%016llx,%016llx,%016llx)\n", *(unsigned long long *)$output_data, *(unsigned long long *)($output_data+1), *(unsigned long long *)($output_data+2)
  kill
  quit
end
run
