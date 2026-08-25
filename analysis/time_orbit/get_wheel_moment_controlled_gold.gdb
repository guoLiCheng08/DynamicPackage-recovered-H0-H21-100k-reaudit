set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/get_wheel_moment_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $g=(char *)&WheelGroup
  set {int}$g=3
  set {int}($g+4)=0
  set *(void **)($g+8)=$g+0x80
  set {double}($g+0x80)=1.25
  set {double}($g+0x88)=-2.5
  set {double}($g+0x90)=3.75
  set {int}($g+0x28)=3
  set {int}($g+0x2c)=0
  set *(void **)($g+0x30)=$g+0xa0
  set {double}($g+0xa0)=0.0
  set {double}($g+0xa8)=0.0
  set {double}($g+0xb0)=0.0
  call (void)GetWheelMoment((void *)($g+0x28))
  printf "GETWHEEL\n"
  x/3gx ($g+0xa0)
  p/x *(int *)($g+0x28)
  p/x (long)*(void **)($g+0x30)-(long)$g
  kill
  quit
end
run
