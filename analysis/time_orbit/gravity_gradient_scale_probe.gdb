set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set $hits = 0
break *GravityGradientTorque+0xef
commands
  silent
  set $hits = $hits + 1
  if $hits == 2
    set $scale_values = (double *)calloc(2,8)
    set {double}$scale_values = $xmm0.v2_double[0]
    set {double}($scale_values+1) = $xmm2.v2_double[0]
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_gravity_step2_scale_operands.bin $scale_values ($scale_values+2)
    x/2gx $scale_values
  end
  continue
end
source /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/core_dynamic_two_step_y_probe.gdb
