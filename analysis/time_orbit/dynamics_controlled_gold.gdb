set pagination off
set confirm off
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/dynamics_controlled_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $out=(char *)calloc(1,16)
  set $outd=(double *)calloc(3,8)
  set *((int *)$out)=3
  set *((void **)($out+8))=$outd
  set $term=(char *)calloc(1,16)
  set $termd=(double *)calloc(3,8)
  set *((int *)$term)=3
  set *((void **)($term+8))=$termd
  set $rate=(char *)calloc(1,16)
  set $rated=(double *)calloc(3,8)
  set *((int *)$rate)=3
  set *((void **)($rate+8))=$rated
  set $add=(char *)calloc(1,16)
  set $addd=(double *)calloc(3,8)
  set *((int *)$add)=3
  set *((void **)($add+8))=$addd
  set $j=(char *)calloc(1,24)
  set $jd=(double *)calloc(9,8)
  set *((int *)$j)=3
  set *((int *)($j+4))=3
  set *((int *)($j+8))=3
  set *((void **)($j+16))=$jd
  set $ji=(char *)calloc(1,24)
  set $jid=(double *)calloc(9,8)
  set *((int *)$ji)=3
  set *((int *)($ji+4))=3
  set *((int *)($ji+8))=3
  set *((void **)($ji+16))=$jid
  set $rated[0]=0.2
  set $rated[1]=-0.3
  set $rated[2]=0.4
  set $addd[0]=1.0
  set $addd[1]=-2.0
  set $addd[2]=0.5
  set $termd[0]=0.1
  set $termd[1]=0.2
  set $termd[2]=-0.3
  set $jd[0]=2.0
  set $jd[4]=3.0
  set $jd[8]=4.0
  set $jid[0]=0.5
  set $jid[4]=0.33333333333333331
  set $jid[8]=0.25
  call (void)dynamics((void *)$out,(void *)$term,(void *)$rate,(void *)$add,(void *)$j,(void *)$ji)
  printf "DYNAMICS_OUT\n"
  x/3gx $outd
  printf "DYNAMICS_TERM\n"
  x/3gx $termd
  kill
  quit
end
run
