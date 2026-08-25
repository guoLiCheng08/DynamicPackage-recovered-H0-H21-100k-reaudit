set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/torque_wheel_moment_short_abi_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $s=(char *)&Sat
  set $l=(char *)&L_c_B
  set $h=(char *)&H_w_B
  set *(void **)($l+8)=$s+0x00
  set *(void **)($h+8)=$s+0x30
  set {int}($s+0x60)=3
  set {int}($s+0x64)=0
  set *(void **)($s+0x68)=$s+0x80
  set {double}($s+0x80)=1.25
  set {double}($s+0x88)=-2.5
  set {double}($s+0x90)=3.75
  set {int}($s+0xa0)=3
  set {int}($s+0xa4)=0
  set *(void **)($s+0xa8)=$s+0xc0
  set {int}($s+0xe0)=3
  set {int}($s+0xe4)=0
  set *(void **)($s+0xe8)=$s+0x100
  set {double}($s+0x100)=-4.125
  set {double}($s+0x108)=5.5
  set {double}($s+0x110)=-6.625
  set {int}($s+0x120)=3
  set {int}($s+0x124)=0
  set *(void **)($s+0x128)=$s+0x140
  call (void)SetTorque((void *)($s+0x60))
  call (void)GetTorque((void *)($s+0xa0))
  call (void)SetWheelAngularMoment((void *)($s+0xe0))
  call (void)GetWheelAngularMoment((void *)($s+0x120))
  printf "SHORT_ABIS\n"
  x/3gx ($s+0x00)
  x/3gx ($s+0xc0)
  x/3gx ($s+0x30)
  x/3gx ($s+0x140)
  kill
  quit
end
run
