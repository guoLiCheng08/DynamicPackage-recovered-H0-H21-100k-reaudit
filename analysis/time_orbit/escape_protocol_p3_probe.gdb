set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  # Original ABI symbol under test: escape_character_procotol.
  set $source=(unsigned char *)calloc(5,1)
  set $source[0]=0x11
  set $source[1]=0x7d
  set $source[2]=0x7e
  set $source[3]=0x7f
  set $source[4]=0x22
  set $out=(unsigned char *)calloc(16,1)
  set $ret=((int (*)(void *,void *,int))escape_character_procotol)($out,$source,5)
  set *(int *)($out+12)=$ret
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/escape_character_procotol_gold.bin $out $out+16
  kill
  quit
end
run
