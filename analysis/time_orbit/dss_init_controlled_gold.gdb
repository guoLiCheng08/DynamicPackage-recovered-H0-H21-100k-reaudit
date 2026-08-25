set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dss_init_controlled_gold.log
set logging overwrite on
set logging enabled on
break *DSS_Init
commands
  silent
  set $d=(char *)&DSS
  set {double}($d+0x70)=1.25
  set {double}($d+0x78)=-2.5
  set {double}($d+0x80)=3.75
  set {double}($d+0x88)=-4.125
  set {double}($d+0x90)=5.5
  set {double}($d+0x98)=-6.625
  set {double}($d+0xa0)=7.875
  set {double}($d+0xa8)=-8.25
  set {double}($d+0xb0)=9.5
  set $d2=$d+0x158
  set {double}($d2+0x70)=-1.5
  set {double}($d2+0x78)=2.25
  set {double}($d2+0x80)=-3.0
  set {double}($d2+0x88)=4.75
  set {double}($d2+0x90)=-5.25
  set {double}($d2+0x98)=6.5
  set {double}($d2+0xa0)=-7.75
  set {double}($d2+0xa8)=8.125
  set {double}($d2+0xb0)=9.875
  set $ret=*(void **)$rsp
  tbreak *$ret
  commands $bpnum
    silent
    printf "DSS\n"
    p/x (long)*(void **)((char *)&DSS+0x50)-(long)&DSS
    p/x (long)*(void **)((char *)&DSS+0x68)-(long)&DSS
    p/x (long)*(void **)((char *)&DSS+0x118)-(long)&DSS
    x/9gx ((char *)&DSS+0xb8)
    x/9gx ((char *)&DSS+0x158+0xb8)
    kill
    quit
  end
  continue
end
run
