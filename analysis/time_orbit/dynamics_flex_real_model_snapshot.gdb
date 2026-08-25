set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/dynamics_flex_real_model_snapshot.log
set logging overwrite on
set logging enabled on
set $seen_entry = 0
break *dynamics_flex
commands
  silent
  set $seen_entry = 1
  set $out_rigid = $rdi
  set $out_modal_a = $rdx
  set $out_modal_v = $r8
  set $state_rate = $rsi
  set $state_modal_a = $rcx
  set $state_modal_v = $r9
  continue
end
break *(dynamics_flex + 0xb36)
commands
  silent
  if $seen_entry == 1
    printf "state rate=(%.17g,%.17g,%.17g) modal_a0=%.17g modal_v0=%.17g\n", *(double *)*(void **)((char *)$state_rate+8), *(double *)(*(void **)((char *)$state_rate+8)+8), *(double *)(*(void **)((char *)$state_rate+8)+16), *(double *)*(void **)((char *)$state_modal_a+8), *(double *)*(void **)((char *)$state_modal_v+8)
    printf "out rigid=(%.17g,%.17g,%.17g) modal_a=(%.17g,%.17g,%.17g)\n", *(double *)*(void **)((char *)$out_rigid+8), *(double *)(*(void **)((char *)$out_rigid+8)+8), *(double *)(*(void **)((char *)$out_rigid+8)+16), *(double *)*(void **)((char *)$out_modal_a+8), *(double *)(*(void **)((char *)$out_modal_a+8)+8), *(double *)(*(void **)((char *)$out_modal_a+8)+16)
    printf "Sat C first=(%.17g,%.17g,%.17g) K00=%.17g D00=%.17g\n", *(double *)*(void **)((char *)&Sat+0x8a8), *(double *)(*(void **)((char *)&Sat+0x8a8)+8), *(double *)(*(void **)((char *)&Sat+0x8a8)+16), *(double *)*(void **)((char *)&Sat+0xa10), *(double *)*(void **)((char *)&Sat+0xd48)
    kill
    quit
  end
  continue
end
run
