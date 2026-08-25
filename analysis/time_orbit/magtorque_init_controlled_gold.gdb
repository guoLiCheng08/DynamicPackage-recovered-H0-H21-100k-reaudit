set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/magtorque_init_controlled_gold.log
set logging overwrite on
set logging enabled on
break *MagTorque_Init
commands
  silent
  set $m=(char *)&MTQ
  set $g=(char *)&MTQ_Group
  set $i=0
  while $i < 6
    set $b=$m+$i*0x38
    set {int}($b+0x10)=3
    set {int}($b+0x14)=0
    set *(void **)($b+0x18)=$b+0x20
    set $i=$i+1
  end
  set {double}($m+0x20)=1.25
  set {double}($m+0x28)=-2.5
  set {double}($m+0x30)=3.75
  set {double}($m+0x58)=4.125
  set {double}($m+0x60)=-5.5
  set {double}($m+0x68)=6.625
  set {double}($m+0x90)=-7.875
  set {double}($m+0x98)=8.25
  set {double}($m+0xa0)=-9.5
  set {double}($m+0xc8)=10.75
  set {double}($m+0xd0)=-11.125
  set {double}($m+0xd8)=12.5
  set {double}($m+0x100)=-13.75
  set {double}($m+0x108)=14.125
  set {double}($m+0x110)=-15.5
  set {double}($m+0x138)=16.75
  set {double}($m+0x140)=-17.25
  set {double}($m+0x148)=18.5
  set {int}($g+0x68)=3
  set {int}($g+0x6c)=6
  set {int}($g+0x70)=6
  set {int}($g+0x74)=0
  set *(void **)($g+0x78)=$g+0x80
  set $ret=*(void **)$rsp
  tbreak *$ret
  commands $bpnum
    silent
    printf "MAGTORQUE\n"
    x/18gx ($g+0x80)
    kill
    quit
  end
  continue
end
run
