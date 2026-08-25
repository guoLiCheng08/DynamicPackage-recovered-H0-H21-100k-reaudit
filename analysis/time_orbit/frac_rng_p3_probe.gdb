set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  # Original ABI symbols under test: Frac and rng_uniform_pos.
  set $frac_inputs = (double *)calloc(6,8)
  set $frac_results = (double *)calloc(6,8)
  set $frac_inputs[0] = 1.25
  set $frac_inputs[1] = -1.25
  set $frac_inputs[2] = 2.0
  set $frac_inputs[3] = -2.0
  set $frac_inputs[4] = 4503599627370496.0
  set *(unsigned long long *)((char *)$frac_inputs+40) = 0x8000000000000000
  set $i = 0
  while $i < 6
    set $frac_results[$i] = ((double (*)(double))Frac)($frac_inputs[$i])
    set $i = $i + 1
  end
  call ((void (*)(unsigned int))srand)(1)
  set $rng_results = (double *)calloc(4,8)
  set $rng_results[0] = ((double (*)(void))rng_uniform_pos)()
  set $rng_results[1] = ((double (*)(void))rng_uniform_pos)()
  set $rng_results[2] = ((double (*)(void))rng_uniform_pos)()
  set $rng_results[3] = ((double (*)(void))rng_uniform_pos)()
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/frac_gold.bin $frac_results $frac_results+6
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/gold_rng_uniform_pos_p3_output.bin $rng_results $rng_results+4
  kill
  quit
end
run
