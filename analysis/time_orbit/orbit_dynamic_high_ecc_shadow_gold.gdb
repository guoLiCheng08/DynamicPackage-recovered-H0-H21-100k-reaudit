set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  call ((void (*)(double, double, double, double, double, double)) TimeInit)(2031.0, 12.0, 31.0, 23.0, 59.0, 50.0)
  set $position_data = (double *)malloc(24)
  set $position_data[0] = -12000000.0
  set $position_data[1] = 65000000.0
  set $position_data[2] = 28000000.0
  set $position_vector = (char *)malloc(16)
  set *(int *)$position_vector = 3
  set *(int *)($position_vector + 4) = 0
  set *(double **)($position_vector + 8) = $position_data
  set $output_data = (double *)malloc(24)
  set $output_vector = (char *)malloc(16)
  set *(int *)$output_vector = 3
  set *(int *)($output_vector + 4) = 0
  set *(double **)($output_vector + 8) = $output_data
  set $force_data = *(double **)((char *)&F_I_external + 8)
  set $force_data[0] = 0.0
  set $force_data[1] = 0.0
  set $force_data[2] = 0.0
  set *(double *)&SpacecraftMass = 1000.0
  call ((void (*)(void *, void *)) orbit_dynamic)($output_vector, $position_vector)
  dump binary memory /home/ubuntu/dynamicpackage_recovered_work/analysis/time_orbit/gold_orbit_dynamic_high_ecc_shadow_acceleration.bin $output_data ($output_data+3)
  printf "acceleration=(%.17g, %.17g, %.17g) bits=(%016llx, %016llx, %016llx)\n", $output_data[0], $output_data[1], $output_data[2], *(unsigned long long *)$output_data, *(unsigned long long *)($output_data + 1), *(unsigned long long *)($output_data + 2)
  kill
  quit
end
run
