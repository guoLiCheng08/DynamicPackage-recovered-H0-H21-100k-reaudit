set disable-randomization on
file /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
break main
commands
  silent
  set $data=(double *)calloc(3,8)
  set {unsigned long long}($data+0)=0xfff8000000000000
  set {unsigned long long}($data+1)=0xfff8000000000000
  set {unsigned long long}($data+2)=0xfff8000000000000
  set $vector=(char *)calloc(0x10,1)
  set {int}$vector=3
  set {void *}($vector+8)=$data
  set $scalar_bits=(unsigned long long *)calloc(1,8)
  set {unsigned long long}$scalar_bits=0xfff8000000000000
  call ((int (*)(void *,double))vector_scale)($vector,*((double *)$scalar_bits))
  dump binary memory /home/ubuntu/dynamicpackage_194_work/dynamicpackage_recovered_work/analysis/math/gold_vector_scale_h0_nan.bin $data ($data+3)
  kill
  quit
end
run
