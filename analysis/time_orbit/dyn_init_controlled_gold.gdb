set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dyn_init_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $p=(char *)calloc(1, 0xe8)
  set {double}($p+0x00)=0.1
  set {double}($p+0x08)=1.0
  set {double}($p+0x10)=0.0
  set {double}($p+0x18)=0.0
  set {double}($p+0x20)=0.0
  set {float}($p+0x28)=0.01
  set {float}($p+0x2c)=-0.02
  set {float}($p+0x30)=0.03
  set {double}($p+0x38)=2024.0
  set {double}($p+0x40)=2.0
  set {double}($p+0x48)=29.0
  set {double}($p+0x50)=23.0
  set {double}($p+0x58)=59.0
  set {double}($p+0x60)=59.5
  set {double}($p+0x68)=7000000.0
  set {double}($p+0x70)=0.01
  set {double}($p+0x78)=0.5
  set {double}($p+0x80)=1.0
  set {double}($p+0x88)=0.2
  set {double}($p+0x90)=0.3
  set {double}($p+0x98)=3.0
  set {double}($p+0xa0)=1.0
  set {double}($p+0xa8)=-2.0
  set {double}($p+0xb0)=0.5
  set {double}($p+0xb8)=4.0
  set {double}($p+0xc0)=1.5
  set {double}($p+0xc8)=-1.0
  set {double}($p+0xd0)=2.0
  set {double}($p+0xd8)=5.0
  set {double}($p+0xe0)=600.0
  call (void)dyn_init((void *)$p)
  printf "DYN_INIT\n"
  x/13gx &y
  set $j=(char *)&J_c_B
  set $ji=(char *)&J_c_B_inv
  x/9gx *(void **)($j+0x10)
  x/9gx *(void **)($ji+0x10)
  x/1gx &SpacecraftMass
  set $gyro=(char *)&Gyro
  p/x (long)*(void **)($gyro+0x78)-(long)$gyro
  kill
  quit
end
run
