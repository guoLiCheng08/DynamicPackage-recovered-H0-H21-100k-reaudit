set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/orbit_dynamic_case2_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  call ((void (*)(double, double, double, double, double, double)) TimeInit)(2024.0, 6.0, 15.0, 12.0, 34.0, 56.0)
  set $position_data = (double *)malloc(24)
  set $position_data[0] = -4431241.4282810194
  set $position_data[1] = -4799941.4308603881
  set $position_data[2] = 2514888.0023961156
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
  set $force_data[0] = 22.0
  set $force_data[1] = -16.0
  set $force_data[2] = 7.0
  set *(double *)&SpacecraftMass = 450.0
  call ((void (*)(void *, void *)) orbit_dynamic)($output_vector, $position_vector)
  printf "time=2024-06-15T12:34:56 position=(%.17g, %.17g, %.17g) force=(22,-16,7) mass=450 acceleration=(%.17g, %.17g, %.17g) bits=(%016llx, %016llx, %016llx)\n", $position_data[0], $position_data[1], $position_data[2], $output_data[0], $output_data[1], $output_data[2], *(unsigned long long *)$output_data, *(unsigned long long *)($output_data + 1), *(unsigned long long *)($output_data + 2)
  kill
  quit
end
run
