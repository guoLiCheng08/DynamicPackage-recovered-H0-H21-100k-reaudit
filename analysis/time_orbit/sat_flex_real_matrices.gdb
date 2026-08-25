set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/sat_flex_real_matrices.log
set logging overwrite on
set logging enabled on
break *dynamics_flex
commands
  silent
  set $m4 = *(double **)((char *)&Sat + 0x4b8)
  set $m6 = *(double **)((char *)&Sat + 0x698)
  set $c = *(double **)((char *)&Sat + 0x8a8)
  set $m9 = *(double **)((char *)&Sat + 0x9b0)
  set $ma = *(double **)((char *)&Sat + 0xa10)
  set $md = *(double **)((char *)&Sat + 0xd48)
  printf "M4_3x3\n"
  x/9gx $m4
  printf "M6_3x10\n"
  x/30gx $m6
  printf "C_3x10\n"
  x/30gx $c
  printf "M9_3x3\n"
  x/9gx $m9
  printf "MA_10x10\n"
  x/100gx $ma
  printf "MD_10x10\n"
  x/100gx $md
  printf "H_w_B L_c_B J_c_B J_c_B_inv\n"
  x/3gx *(void **)((char *)&H_w_B + 8)
  x/3gx *(void **)((char *)&L_c_B + 8)
  x/9gx *(void **)((char *)&J_c_B + 16)
  x/9gx *(void **)((char *)&J_c_B_inv + 16)
  kill
  quit
end
run
