set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/orbit_dynamic_component_trace.log
set logging overwrite on
set logging enabled on
dprintf *orbit_dynamic+0x487, "ecef position=(%.17g, %.17g, %.17g) central=(%.17g, %.17g, %.17g) term1=(%.17g, %.17g, %.17g) term2=(%.17g, %.17g, %.17g) term3=(%.17g, %.17g, %.17g)\n", *(double *)($rsp+0xd0), *(double *)($rsp+0xd8), *(double *)($rsp+0xe0), *(double *)($rsp+0xf0), *(double *)($rsp+0xf8), *(double *)($rsp+0x100), *(double *)($rsp+0x110), *(double *)($rsp+0x118), *(double *)($rsp+0x120), *(double *)($rsp+0x130), *(double *)($rsp+0x138), *(double *)($rsp+0x140), *(double *)($rsp+0x150), *(double *)($rsp+0x158), *(double *)($rsp+0x160)
dprintf *orbit_dynamic+0x49f, "ecef accumulated=(%.17g, %.17g, %.17g)\n", *(double *)($rsp+0x170), *(double *)($rsp+0x178), *(double *)($rsp+0x180)
break main
commands
  silent
  call ((void (*)(double, double, double, double, double, double)) TimeInit)(2020.0, 1.0, 2.0, 3.0, 4.0, 5.0)
  set $position_data = (double *)malloc(24)
  set $position_data[0] = 7000000.0
  set $position_data[1] = -1210000.0
  set $position_data[2] = 2300000.0
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
  printf "final gci acceleration=(%.17g, %.17g, %.17g)\n", $output_data[0], $output_data[1], $output_data[2]
  kill
  quit
end
run
