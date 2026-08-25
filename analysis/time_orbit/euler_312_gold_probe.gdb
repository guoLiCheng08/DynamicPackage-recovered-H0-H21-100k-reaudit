set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/euler_312_gold_probe.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  # Original ABI symbol under test: euler_angle_312_to_quat.
  set $q = (char *)calloc(0x20,1)
  set $qxyz = (double *)calloc(3,8)
  set {int}($q+0x08) = 3
  set {void **}($q+0x10) = $qxyz
  call ((void (*)(void *,double,double,double))euler_angle_312_to_quat)($q,0.3,-0.4,0.7)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/euler_angle_312_to_quat_gold.bin $q $q+24
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/euler_angle_312_to_quat_xyz_gold.bin $qxyz $qxyz+3
  x/gx $q
  x/3gx $qxyz
  kill
  quit
end
run
