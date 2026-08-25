set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/rk4_quaternion_stage_derivative_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $initial = (double *)calloc(1, 232)
  set $initial[0] = 0.1
  set $initial[7] = 2020.0
  set $initial[8] = 1.0
  set $initial[9] = 2.0
  set $initial[10] = 3.0
  set $initial[11] = 4.0
  set $initial[12] = 5.0
  set $initial[13] = 7000000.0
  set $initial[14] = 0.001
  set $initial[15] = 0.1
  set $initial[16] = 0.2
  set $initial[17] = 0.3
  set $initial[18] = 0.4
  set $initial[19] = 10.0
  set $initial[23] = 11.0
  set $initial[27] = 12.0
  set $initial[28] = 100.0
  call ((void (*)(void *)) DynamicInit)($initial)
  set $out = (double *)calloc(33, 8)

  set *(unsigned long long *)((double *)&y + 0) = 0x3ff0000000000000
  set *(unsigned long long *)((double *)&y + 1) = 0x0000000000000000
  set *(unsigned long long *)((double *)&y + 2) = 0x0000000000000000
  set *(unsigned long long *)((double *)&y + 3) = 0x0000000000000000
  set *(unsigned long long *)((double *)&y + 4) = 0x3f847ae147ae147b
  set *(unsigned long long *)((double *)&y + 5) = 0xbf947ae147ae147b
  set *(unsigned long long *)((double *)&y + 6) = 0x3f9eb851eb851eb8
  call ((void (*)(void *, void *, double)) differential_equation)($out, (double *)&y, 0.0)
  printf "stage0_qdot_bits\n"
  x/4gx $out

  set *(unsigned long long *)((double *)&y + 0) = 0x3ff0000000000000
  set *(unsigned long long *)((double *)&y + 1) = 0x3f30624dd2f1a9fc
  set *(unsigned long long *)((double *)&y + 2) = 0xbf40624dd2f1a9fc
  set *(unsigned long long *)((double *)&y + 3) = 0x3f489374bc6a7efa
  set *(unsigned long long *)((double *)&y + 4) = 0x3f84852a60d9aa7b
  set *(unsigned long long *)((double *)&y + 5) = 0xbf947c262aa3667a
  set *(unsigned long long *)((double *)&y + 6) = 0x3f9eb971d522db97
  call ((void (*)(void *, void *, double)) differential_equation)($out, (double *)&y, 0.0)
  printf "stage1_qdot_bits\n"
  x/4gx $out

  set *(unsigned long long *)((double *)&y + 0) = 0x3feffffe2a18be5b
  set *(unsigned long long *)((double *)&y + 1) = 0x3f306a87fdd36f1f
  set *(unsigned long long *)((double *)&y + 2) = 0xbf40635087f11782
  set *(unsigned long long *)((double *)&y + 3) = 0x3f48945ad2accddd
  set *(unsigned long long *)((double *)&y + 4) = 0x3f8485842e3c8bd1
  set *(unsigned long long *)((double *)&y + 5) = 0xbf947c3881f6128b
  set *(unsigned long long *)((double *)&y + 6) = 0x3f9eb97be0010366
  call ((void (*)(void *, void *, double)) differential_equation)($out, (double *)&y, 0.0)
  printf "stage2_qdot_bits\n"
  x/4gx $out

  set *(unsigned long long *)((double *)&y + 0) = 0x3feffffc53e5ff25
  set *(unsigned long long *)((double *)&y + 1) = 0x3f406acfb0641654
  set *(unsigned long long *)((double *)&y + 2) = 0xbf50635fe935aeed
  set *(unsigned long long *)((double *)&y + 3) = 0x3f58946268ae121a
  set *(unsigned long long *)((double *)&y + 4) = 0x3f8490307fc097cf
  set *(unsigned long long *)((double *)&y + 5) = 0xbf947d91b7180034
  set *(unsigned long long *)((double *)&y + 6) = 0x3f9ebaa6dd55f8ee
  call ((void (*)(void *, void *, double)) differential_equation)($out, (double *)&y, 0.0)
  printf "stage3_qdot_bits\n"
  x/4gx $out
  kill
  quit
end
run
