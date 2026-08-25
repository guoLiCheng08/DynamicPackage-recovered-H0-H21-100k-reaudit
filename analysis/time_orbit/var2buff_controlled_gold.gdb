set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/var2buff_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $source=(unsigned char *)calloc(5,1)
  set $out_native=(unsigned char *)malloc(5)
  set $out_reverse=(unsigned char *)malloc(5)
  set $out_other=(unsigned char *)malloc(5)
  set $source[0]=0x12
  set $source[1]=0x34
  set $source[2]=0x56
  set $source[3]=0x78
  set $source[4]=0x9a
  call (void)memset($out_native,0xa5,5)
  call (void)memset($out_reverse,0xa5,5)
  call (void)memset($out_other,0xa5,5)
  call ((void (*)(void *, void *, int, unsigned char))var2buff)((void *)$out_native,(void *)$source,5,1)
  call ((void (*)(void *, void *, int, unsigned char))var2buff)((void *)$out_reverse,(void *)$source,5,0)
  call ((void (*)(void *, void *, int, unsigned char))var2buff)((void *)$out_other,(void *)$source,5,2)
  printf "VAR2BUFF_NATIVE_REVERSE_OTHER\n"
  x/5bx $out_native
  x/5bx $out_reverse
  x/5bx $out_other
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/var2buff_gold_native.bin $out_native $out_native+5
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/var2buff_gold_reverse.bin $out_reverse $out_reverse+5
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/var2buff_gold_other.bin $out_other $out_other+5
  kill
  quit
end
run
