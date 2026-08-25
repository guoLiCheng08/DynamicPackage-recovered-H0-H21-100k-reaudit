set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_restore/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set $hits = 0
break *GravityGradientTorque+0xc0
commands
  silent
  set $hits = $hits + 1
  if $hits == 2
    set $unit = (double *)($rsp+0x50)
    set $junit = (double *)($rsp+0x30)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_gravity_step2_unit.bin $unit ($unit+3)
    dump binary memory /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/gold_gravity_step2_junit.bin $junit ($junit+3)
    x/3gx $unit
    x/3gx $junit
    kill
    quit
  end
  continue
end
source /home/ubuntu/dynamicpackage_restore/analysis/time_orbit/core_dynamic_two_step_y_probe.gdb
