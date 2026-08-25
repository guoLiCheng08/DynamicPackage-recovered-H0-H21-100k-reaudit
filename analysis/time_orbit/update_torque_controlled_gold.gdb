set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/update_torque_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $t=(char *)&SatTorque
  set $l=(char *)&L_c_B
  call (void)TorqueInit()
  set {int}($t+0x00)=3
  set {int}($t+0x04)=0
  set {int}($t+0x10)=3
  set {int}($t+0x14)=0
  set {int}($t+0x20)=3
  set {int}($t+0x24)=0
  set {int}($t+0x70)=3
  set {int}($t+0x74)=0
  set {double}($t+0x80)=1.25
  set {double}($t+0x88)=-2.5
  set {double}($t+0x90)=3.75
  set {double}($t+0x98)=-4.0
  set {double}($t+0xa0)=5.5
  set {double}($t+0xa8)=6.25
  set {double}($t+0xb0)=7.0
  set {double}($t+0xb8)=-8.5
  set {double}($t+0xc0)=9.125
  set {int}$l=3
  set {int}($l+4)=0
  set *(void **)($l+8)=$t+0x300
  call (void)UpdateTorque()
  printf "UPDATE_TORQUE\n"
  x/3gx ($t+0x128)
  x/3gx ($t+0x300)
  p/x *(int *)($t+0x70)
  p/x (long)*(void **)($t+0x78)-(long)$t
  kill
  quit
end
run
