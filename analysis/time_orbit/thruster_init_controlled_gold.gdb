set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/thruster_init_controlled_gold.log
set logging overwrite on
set logging enabled on
break *Thruster_Init
commands
  silent
  set $t=(char *)&Thruster
  set {long long}($t+0x00)=0x1122334455667788
  set {int}($t+0x08)=0x12345678
  set {long long}($t+0x10)=0x0000000300000003
  set {long long}($t+0x18)=0
  set {long long}($t+0x38)=0x0000000300000003
  set {long long}($t+0x40)=0
  set {long long}($t+0x60)=0x0000000300000003
  set {long long}($t+0x68)=0
  set {long long}($t+0x88)=0x0000000300000003
  set {long long}($t+0x90)=0
  set $ret=*(void **)$rsp
  tbreak *$ret
  commands $bpnum
    silent
    printf "THRUSTER\n"
    p/x (long)*(void **)((char *)&Thruster+0x18)-(long)&Thruster
    p/x (long)*(void **)((char *)&Thruster+0x40)-(long)&Thruster
    p/x (long)*(void **)((char *)&Thruster+0x68)-(long)&Thruster
    p/x (long)*(void **)((char *)&Thruster+0x90)-(long)&Thruster
    x/22gx &Thruster
    kill
    quit
  end
  continue
end
run
