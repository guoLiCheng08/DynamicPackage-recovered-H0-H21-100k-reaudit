set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  # Original ABI symbols under test: s2 and c2.
  set $s2out=(double *)calloc(5,8)
  set $c2out=(double *)calloc(5,8)
  set $s2fn=(double (*)(double))($pc+0x7eb0)
  set $c2fn=(double (*)(double))($pc+0x7ec0)
  set $s2out[0]=$s2fn(0.0)
  set $s2out[1]=$s2fn(1.0)
  set $s2out[2]=$s2fn(-1.0)
  set $s2out[3]=$s2fn(6.283185307179586)
  set $s2out[4]=$s2fn(-6.283185307179586)
  set $c2out[0]=$c2fn(0.0)
  set $c2out[1]=$c2fn(1.0)
  set $c2out[2]=$c2fn(-1.0)
  set $c2out[3]=$c2fn(6.283185307179586)
  set $c2out[4]=$c2fn(-6.283185307179586)
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/s2_gold.bin $s2out $s2out+5
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/c2_gold.bin $c2out $c2out+5
  kill
  quit
end
run
