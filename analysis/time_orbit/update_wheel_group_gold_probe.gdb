set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/update_wheel_group_gold_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  call ((void (*)(void)) Wheel_Init)()
  printf "wheel_group_descriptors\n"
  printf "v0 n=%d p=%p v28 n=%d p=%p matrix50=(%d,%d,%d) p=%p\n", *(int *)((char *)&WheelGroup+0x00), *(void **)((char *)&WheelGroup+0x08), *(int *)((char *)&WheelGroup+0x28), *(void **)((char *)&WheelGroup+0x30), *(int *)((char *)&WheelGroup+0x50), *(int *)((char *)&WheelGroup+0x54), *(int *)((char *)&WheelGroup+0x58), *(void **)((char *)&WheelGroup+0x60)
  set $matrix = *(double **)((char *)&WheelGroup+0x60)
  set $w = (char *)&RWheel
  set $matrix[0] = 1.0
  set $matrix[1] = 0.0
  set $matrix[2] = 0.0
  set $matrix[3] = 0.0
  set $matrix[4] = 0.0
  set $matrix[5] = 1.0
  set $matrix[6] = 0.0
  set $matrix[7] = 0.0
  set $matrix[8] = 0.0
  set $matrix[9] = 0.0
  set $matrix[10] = 1.0
  set $matrix[11] = 0.0
  set *(double *)($w + 0x28) = 0.1
  set *(double *)($w + 0xa0) = 0.1
  set *(double *)($w + 0x118) = 0.1
  set *(double *)($w + 0x190) = 0.1
  set *(double *)($w + 0x30) = 0.3
  set *(double *)($w + 0xa8) = -0.4
  set *(double *)($w + 0x120) = 0.5
  set *(double *)($w + 0x198) = -0.6
  set *(double *)($w + 0x38) = 0.2
  set *(double *)($w + 0xb0) = -0.4
  set *(double *)($w + 0x128) = 0.5
  set *(double *)($w + 0x1a0) = -0.5
  call ((void (*)(double)) UpdateWheel)(0.2)
  printf "wheel_group_v0_v28_bits\n"
  x/3gx *(void **)((char *)&WheelGroup+0x08)
  x/3gx *(void **)((char *)&WheelGroup+0x30)
  printf "wheel_fields_bits\n"
  x/3gx ($w + 0x30)
  x/3gx ($w + 0xa8)
  x/3gx ($w + 0x120)
  x/3gx ($w + 0x198)
  kill
  quit
end
run
