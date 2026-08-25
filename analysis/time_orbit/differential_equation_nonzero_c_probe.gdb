set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/differential_equation_nonzero_c_probe.log
set logging overwrite on
set logging enabled on
dprintf *dynamics_flex, "dynamics_flex args rdi=%p rsi=%p rdx=%p rcx=%p r8=%p r9=%p stack7=%p stack8=%p stack9=%p stack10=%p\n", $rdi, $rsi, $rdx, $rcx, $r8, $r9, *(void **)($rsp + 8), *(void **)($rsp + 16), *(void **)($rsp + 24), *(void **)($rsp + 32)
dprintf *dynamics_flex, "containers a0=(n=%d data=%p v0=%.17g) a1=(n=%d data=%p v0=%.17g) a2=(n=%d data=%p v0=%.17g) a3=(n=%d data=%p v0=%.17g) a4=(n=%d data=%p v0=%.17g) a5=(n=%d data=%p v0=%.17g)\n", *(int *)$rdi, *(void **)((char *)$rdi+8), *(double *)*(void **)((char *)$rdi+8), *(int *)$rsi, *(void **)((char *)$rsi+8), *(double *)*(void **)((char *)$rsi+8), *(int *)$rdx, *(void **)((char *)$rdx+8), *(double *)*(void **)((char *)$rdx+8), *(int *)$rcx, *(void **)((char *)$rcx+8), *(double *)*(void **)((char *)$rcx+8), *(int *)$r8, *(void **)((char *)$r8+8), *(double *)*(void **)((char *)$r8+8), *(int *)$r9, *(void **)((char *)$r9+8), *(double *)*(void **)((char *)$r9+8)
dprintf *dynamics_flex, "Sat.C rows=%d cols=%d stride=%d data=%p first=%.17g\n", *(int *)((char *)&Sat+0x898), *(int *)((char *)&Sat+0x89c), *(int *)((char *)&Sat+0x8a0), *(void **)((char *)&Sat+0x8a8), *(double *)*(void **)((char *)&Sat+0x8a8)
dprintf *dynamics_flex, "Sat matrices m4a8=(%d,%d,%d,%.17g) m688=(%d,%d,%d,%.17g) m9a0=(%d,%d,%d,%.17g) ma00=(%d,%d,%d,%.17g) md38=(%d,%d,%d,%.17g)\n", *(int *)((char *)&Sat+0x4a8), *(int *)((char *)&Sat+0x4ac), *(int *)((char *)&Sat+0x4b0), *(double *)*(void **)((char *)&Sat+0x4b8), *(int *)((char *)&Sat+0x688), *(int *)((char *)&Sat+0x68c), *(int *)((char *)&Sat+0x690), *(double *)*(void **)((char *)&Sat+0x698), *(int *)((char *)&Sat+0x9a0), *(int *)((char *)&Sat+0x9a4), *(int *)((char *)&Sat+0x9a8), *(double *)*(void **)((char *)&Sat+0x9b0), *(int *)((char *)&Sat+0xa00), *(int *)((char *)&Sat+0xa04), *(int *)((char *)&Sat+0xa08), *(double *)*(void **)((char *)&Sat+0xa10), *(int *)((char *)&Sat+0xd38), *(int *)((char *)&Sat+0xd3c), *(int *)((char *)&Sat+0xd40), *(double *)*(void **)((char *)&Sat+0xd48)
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
  set $c_data = *(double **)((char *)&Sat + 0x8a8)
  set $c_data[0] = 0.1
  set $c_data[10] = -0.05
  set $c_data[20] = 0.025
  set ((double *)&y)[0] = 1.0
  set ((double *)&y)[1] = 0.0
  set ((double *)&y)[2] = 0.0
  set ((double *)&y)[3] = 0.0
  set ((double *)&y)[4] = 0.01
  set ((double *)&y)[5] = -0.02
  set ((double *)&y)[6] = 0.03
  set ((double *)&y)[13] = 0.001
  set ((double *)&y)[14] = -0.002
  set ((double *)&y)[15] = 0.003
  set ((double *)&y)[16] = -0.004
  set ((double *)&y)[17] = 0.005
  set ((double *)&y)[18] = -0.006
  set ((double *)&y)[19] = 0.007
  set ((double *)&y)[20] = -0.008
  set ((double *)&y)[21] = 0.009
  set ((double *)&y)[22] = -0.010
  set ((double *)&y)[23] = 0.011
  set ((double *)&y)[24] = -0.012
  set ((double *)&y)[25] = 0.013
  set ((double *)&y)[26] = -0.014
  set ((double *)&y)[27] = 0.015
  set ((double *)&y)[28] = -0.016
  set ((double *)&y)[29] = 0.017
  set ((double *)&y)[30] = -0.018
  set ((double *)&y)[31] = 0.019
  set ((double *)&y)[32] = -0.020
  set $out = (double *)malloc(264)
  call ((void (*)(void *, void *, double)) differential_equation)($out, (double *)&y, 0.0)
  printf "dydt[0..12]=(%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g)\n", $out[0],$out[1],$out[2],$out[3],$out[4],$out[5],$out[6],$out[7],$out[8],$out[9],$out[10],$out[11],$out[12]
  printf "dydt[13..22]=(%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g)\n", $out[13],$out[14],$out[15],$out[16],$out[17],$out[18],$out[19],$out[20],$out[21],$out[22]
  printf "dydt[23..32]=(%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g,%.17g)\n", $out[23],$out[24],$out[25],$out[26],$out[27],$out[28],$out[29],$out[30],$out[31],$out[32]
  kill
  quit
end
run
