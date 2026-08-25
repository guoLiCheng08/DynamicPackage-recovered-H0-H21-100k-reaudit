set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/sts_init_controlled_gold.log
set logging overwrite on
set logging enabled on
break *STS_Init
commands
  silent
  set $s=(char *)&STS
  set $i=0
  while $i < 3
    set $b=$s+$i*0x170
    set {int}($b+0x60)=3
    set {int}($b+0x64)=3
    set {int}($b+0x68)=3
    set {int}($b+0x6c)=0
    set {double}($b+0x158)=123.25+$i
    set {double}($b+0x160)=-456.5-$i
    set {double}($b+0x168)=789.75+$i
    set $i=$i+1
  end
  set {double}($s+0x78)=1.0
  set {double}($s+0x80)=0.0
  set {double}($s+0x88)=0.0
  set {double}($s+0x90)=0.0
  set {double}($s+0x98)=1.0
  set {double}($s+0xa0)=0.0
  set {double}($s+0xa8)=0.0
  set {double}($s+0xb0)=0.0
  set {double}($s+0xb8)=1.0
  set $s1=$s+0x170
  set {double}($s1+0x78)=1.0
  set {double}($s1+0x80)=0.0
  set {double}($s1+0x88)=0.0
  set {double}($s1+0x90)=0.0
  set {double}($s1+0x98)=-1.0
  set {double}($s1+0xa0)=0.0
  set {double}($s1+0xa8)=0.0
  set {double}($s1+0xb0)=0.0
  set {double}($s1+0xb8)=-1.0
  set $s2=$s+0x2e0
  set {double}($s2+0x78)=0.0
  set {double}($s2+0x80)=-1.0
  set {double}($s2+0x88)=0.0
  set {double}($s2+0x90)=1.0
  set {double}($s2+0x98)=0.0
  set {double}($s2+0xa0)=0.0
  set {double}($s2+0xa8)=0.0
  set {double}($s2+0xb0)=0.0
  set {double}($s2+0xb8)=1.0
  set $ret=*(void **)$rsp
  tbreak *$ret
  commands $bpnum
    silent
    printf "STS\n"
    set $j=0
    while $j < 3
      set $q=(char *)&STS+$j*0x170
      p/x (long)*(void **)($q+0x70)-(long)$q
      p/x (long)*(void **)($q+0xd0)-(long)$q
      p/x (long)*(void **)($q+0x108)-(long)$q
      p/x (long)*(void **)($q+0x150)-(long)$q
      x/1gx ($q+0xc0)
      x/3gx ($q+0xe0)
      x/1gx ($q+0xf8)
      x/3gx ($q+0x118)
      x/1gx ($q+0x140)
      x/3gx ($q+0x158)
      set $j=$j+1
    end
    kill
    quit
  end
  continue
end
run
