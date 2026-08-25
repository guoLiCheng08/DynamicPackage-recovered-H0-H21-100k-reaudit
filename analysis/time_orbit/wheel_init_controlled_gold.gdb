set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/wheel_init_controlled_gold.log
set logging overwrite on
set logging enabled on
break *Wheel_Init
commands
  silent
  set $w=(char *)&RWheel
  set $g=(char *)&WheelGroup
  set $i=0
  while $i < 4
    set $b=$w+$i*0x78
    set {int}($b+0x50)=3
    set {int}($b+0x54)=0
    set *(void **)($b+0x58)=$b
    set $i=$i+1
  end
  set {double}($w+0x00)=1.25
  set {double}($w+0x08)=-2.5
  set {double}($w+0x10)=3.75
  set {double}($w+0x78)=4.125
  set {double}($w+0x80)=-5.5
  set {double}($w+0x88)=6.625
  set {double}($w+0xf0)=-7.875
  set {double}($w+0xf8)=8.25
  set {double}($w+0x100)=-9.5
  set {double}($w+0x168)=10.75
  set {double}($w+0x170)=-11.125
  set {double}($w+0x178)=12.5
  set {int}($g+0x50)=3
  set {int}($g+0x54)=4
  set {int}($g+0x58)=4
  set {int}($g+0x5c)=0
  set *(void **)($g+0x60)=$g+0x68
  set $ret=*(void **)$rsp
  tbreak *$ret
  commands $bpnum
    silent
    printf "WHEEL\n"
    x/12gx ($g+0x68)
    kill
    quit
  end
  continue
end
run
