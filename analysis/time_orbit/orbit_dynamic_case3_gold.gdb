set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/orbit_dynamic_case3_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  call ((void (*)(double, double, double, double, double, double)) TimeInit)(2030.0, 12.0, 31.0, 23.0, 59.0, 30.0)
  set $position_data = (double *)malloc(24)
  set $position_data[0] = 7100000.0
  set $position_data[1] = -100.0
  set $position_data[2] = 10.0
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
  set *(double *)&SpacecraftMass = 100.0
  call ((void (*)(void *, void *)) orbit_dynamic)($output_vector, $position_vector)
  printf "time=2030-12-31T23:59:30 position=(7100000,-100,10) acceleration=(%.17g, %.17g, %.17g) bits=(%016llx, %016llx, %016llx)\n", $output_data[0], $output_data[1], $output_data[2], *(unsigned long long *)$output_data, *(unsigned long long *)($output_data + 1), *(unsigned long long *)($output_data + 2)
  kill
  quit
end
run
