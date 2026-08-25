set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  # Original ABI symbol under test: Sun.
  set $out=(char *)calloc(16,1)
  set $outdata=(double *)calloc(3,8)
  set *((int *)$out)=3
  set *((void **)($out+8))=$outdata
  set $jd=(double *)calloc(1,8)
  set *(unsigned long long *)$jd=0x4142c602d05cfe8f
  call (void)Sun((void *)$out,*$jd)
  set $values=(double *)calloc(4,8)
  set $values[0]=*$jd
  set $values[1]=$outdata[0]
  set $values[2]=$outdata[1]
  set $values[3]=$outdata[2]
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/sun_gold.bin $values $values+4
  kill
  quit
end
run
