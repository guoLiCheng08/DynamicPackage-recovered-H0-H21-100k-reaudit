set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/set_sat_inertia_tensor_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $s=(char *)&Sat
  set {int}($s+0x08)=3
  set {int}($s+0x0c)=3
  set {int}($s+0x10)=3
  set {int}($s+0x14)=0
  set *(void **)($s+0x18)=$s+0x100
  set {int}($s+0x68)=3
  set {int}($s+0x6c)=3
  set {int}($s+0x70)=3
  set {int}($s+0x74)=0
  set *(void **)($s+0x78)=$s+0x200
  set {double}($s+0x300)=3.0
  set {double}($s+0x308)=1.0
  set {double}($s+0x310)=-2.0
  set {double}($s+0x318)=0.5
  set {double}($s+0x320)=4.0
  set {double}($s+0x328)=1.5
  set {double}($s+0x330)=-1.0
  set {double}($s+0x338)=2.0
  set {double}($s+0x340)=5.0
  call (void)SetSatInertiaTensor((void *)($s+0x300))
  printf "SAT_INERTIA\n"
  x/9gx ($s+0x100)
  x/9gx ($s+0x200)
  kill
  quit
end
run
