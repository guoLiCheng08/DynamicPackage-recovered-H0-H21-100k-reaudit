set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/set_inertia_tensor_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $s=(char *)&Sat
  set $j=(char *)&J_c_B
  set $ji=(char *)&J_c_B_inv
  set *(void **)($j+0x10)=$s+0x100
  set *(void **)($ji+0x10)=$s+0x200
  set {double}($s+0x300)=3.0
  set {double}($s+0x308)=1.0
  set {double}($s+0x310)=-2.0
  set {double}($s+0x318)=0.5
  set {double}($s+0x320)=4.0
  set {double}($s+0x328)=1.5
  set {double}($s+0x330)=-1.0
  set {double}($s+0x338)=2.0
  set {double}($s+0x340)=5.0
  call (void)SetInertiaTensor((void *)($s+0x300))
  printf "INERTIA\n"
  x/9gx ($s+0x100)
  x/9gx ($s+0x200)
  kill
  quit
end
run
