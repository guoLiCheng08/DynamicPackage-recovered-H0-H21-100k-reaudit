set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/differential_equation_real_model_snapshot.log
set logging overwrite on
set logging enabled on
set $seen = 0

break *differential_equation
commands
  silent
  if $seen == 0
    set $seen = 1
    set $snapshot_output = (double *)$rdi
    set $snapshot_state = (double *)$rsi
  end
  continue
end

break *(differential_equation + 0x644)
commands
  silent
  if $seen == 1
    printf "state_bits\n"
    x/33gx $snapshot_state
    printf "derivative_bits\n"
    x/33gx $snapshot_output
    printf "mass=%.17g H_bits\n", *(double *)&SpacecraftMass
    x/3gx *(void **)((char *)&H_w_B + 8)
    printf "external_force_bits\n"
    x/3gx *(void **)((char *)&F_I_external + 8)
    kill
    quit
  end
  continue
end
run
