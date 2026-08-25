set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/attitude_p1_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $s=(char *)&Sat
  set {double}&y=0.5
  set {double}((char *)&y+8)=-0.5
  set {double}((char *)&y+16)=0.5
  set {double}((char *)&y+24)=-0.5
  set {double}((char *)&y+32)=1.25
  set {double}((char *)&y+40)=-2.5
  set {double}((char *)&y+48)=3.75
  set {int}($s+0x108)=3
  set {int}($s+0x10c)=0
  set *(void **)($s+0x110)=$s+0x130
  set {int}($s+0x160)=3
  set {int}($s+0x164)=0
  set *(void **)($s+0x168)=$s+0x170
  set {int}($s+0x200)=3
  set {int}($s+0x204)=3
  set {int}($s+0x208)=3
  set {int}($s+0x20c)=0
  set *(void **)($s+0x210)=$s+0x220
  set {int}($s+0x280)=3
  set {int}($s+0x284)=3
  set {int}($s+0x288)=3
  set {int}($s+0x28c)=0
  set *(void **)($s+0x290)=$s+0x2a0
  call (void)get_attitude((void *)($s+0x100), (void *)($s+0x160))
  call (void)get_Inertial2Body((void *)($s+0x200))
  call (void)get_Body2Inertial((void *)($s+0x280))
  printf "ATTITUDE\n"
  x/1gx ($s+0x100)
  x/3gx ($s+0x130)
  x/3gx ($s+0x170)
  x/9gx ($s+0x220)
  x/9gx ($s+0x2a0)
  set $attitude_values=(double *)calloc(7,8)
  set $attitude_values[0]=*(double *)($s+0x100)
  set $attitude_values[1]=*(double *)($s+0x130)
  set $attitude_values[2]=*(double *)($s+0x138)
  set $attitude_values[3]=*(double *)($s+0x140)
  set $attitude_values[4]=*(double *)($s+0x170)
  set $attitude_values[5]=*(double *)($s+0x178)
  set $attitude_values[6]=*(double *)($s+0x180)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/get_attitude_gold.bin $attitude_values $attitude_values+7
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/get_inertial2body_gold.bin $s+0x220 $s+0x220+72
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/get_body2inertial_gold.bin $s+0x2a0 $s+0x2a0+72
  kill
  quit
end
run
