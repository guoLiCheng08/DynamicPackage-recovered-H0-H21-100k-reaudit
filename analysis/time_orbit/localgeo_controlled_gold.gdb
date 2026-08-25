set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/localgeo_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $outmat=(char *)calloc(1,24)
  set $outmatdata=(double *)calloc(9,8)
  set *((int *)$outmat)=3
  set *((int *)($outmat+4))=3
  set *((int *)($outmat+8))=3
  set *((void **)($outmat+16))=$outmatdata
  set $position=(char *)calloc(1,16)
  set $positiondata=(double *)calloc(3,8)
  set *((int *)$position)=3
  set *((void **)($position+8))=$positiondata
  set $positiondata[0]=4.0
  set $positiondata[1]=-3.0
  set $positiondata[2]=12.0
  set $input=(char *)calloc(1,16)
  set $inputdata=(double *)calloc(3,8)
  set *((int *)$input)=3
  set *((void **)($input+8))=$inputdata
  set $inputdata[0]=7.0
  set $inputdata[1]=-11.0
  set $inputdata[2]=13.0
  set $reference=(char *)calloc(1,16)
  set $referencedata=(double *)calloc(3,8)
  set *((int *)$reference)=3
  set *((void **)($reference+8))=$referencedata
  set $referencedata[0]=1.5
  set $referencedata[1]=-2.0
  set $referencedata[2]=0.5
  set $out=(char *)calloc(1,16)
  set $outdata=(double *)calloc(3,8)
  set *((int *)$out)=3
  set *((void **)($out+8))=$outdata
  call (void)R_ECEF2LocalGeo((void *)$outmat, (void *)$position)
  printf "R_ECEF2LOCALGEO\n"
  x/9gx $outmatdata
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/recef2localgeo_gold.bin $outmatdata $outmatdata+9
  call (void)LocalGeo2LVLH((void *)$out, (void *)$input, (void *)$position, (void *)$reference)
  printf "LOCALGEO2LVLH\n"
  x/3gx $outdata
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/localgeo2lvlh_gold.bin $outdata $outdata+3
  kill
  quit
end
run
