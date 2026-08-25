set pagination off
set confirm off
set print elements 64
set disable-randomization on
file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/debug_runner/DynamicPackage.exec_copy
set inferior-tty /dev/null
set logging file /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/posvel2elements_function_gold.log
set logging overwrite on
set logging enabled on
break main
commands
  silent
  set $a = (double *) malloc(8)
  set $e = (double *) malloc(8)
  set $i = (double *) malloc(8)
  set $raan = (double *) malloc(8)
  set $argp = (double *) malloc(8)
  set $true_anomaly = (double *) malloc(8)
  set $mean_anomaly = (double *) malloc(8)
  set $argument_of_latitude = (double *) malloc(8)
  set $position = (double *) malloc(24)
  set $velocity = (double *) malloc(24)
  set $position[0] = -4073306.892903815
  set $position[1] = -5851722.1589736138
  set $position[2] = 1987412.5991544391
  set $velocity[0] = 3643.5044963902051
  set $velocity[1] = -4634.4388877243673
  set $velocity[2] = -4001.236670030642
  call ((void (*)(void *, void *, void *, void *, void *, void *, void *, void *, void *, void *)) PosVel2Elements_M)($a, $e, $i, $raan, $argp, $true_anomaly, $mean_anomaly, $argument_of_latitude, $position, $velocity)
  printf "posvel2elements case0 a=%.17g e=%.17g i=%.17g raan=%.17g argp=%.17g true=%.17g M=%.17g extra=%.17g\n", $a[0], $e[0], $i[0], $raan[0], $argp[0], $true_anomaly[0], $mean_anomaly[0], $argument_of_latitude[0]
  set $position[0] = 27327503.58092948
  set $position[1] = -51840348.8362469
  set $position[2] = -3347546.980123505
  set $velocity[0] = 932.97950821071584
  set $velocity[1] = 1763.3612138445401
  set $velocity[2] = 384.70799507620745
  call ((void (*)(void *, void *, void *, void *, void *, void *, void *, void *, void *, void *)) PosVel2Elements_M)($a, $e, $i, $raan, $argp, $true_anomaly, $mean_anomaly, $argument_of_latitude, $position, $velocity)
  printf "posvel2elements case1 a=%.17g e=%.17g i=%.17g raan=%.17g argp=%.17g true=%.17g M=%.17g extra=%.17g\n", $a[0], $e[0], $i[0], $raan[0], $argp[0], $true_anomaly[0], $mean_anomaly[0], $argument_of_latitude[0]
  set $out=(double *)calloc(16,8)
  set $out[0]=$a[0]
  set $out[1]=$e[0]
  set $out[2]=$i[0]
  set $out[3]=$raan[0]
  set $out[4]=$argp[0]
  set $out[5]=$true_anomaly[0]
  set $out[6]=$mean_anomaly[0]
  set $out[7]=$argument_of_latitude[0]
  set $position[0] = -4073306.892903815
  set $position[1] = -5851722.1589736138
  set $position[2] = 1987412.5991544391
  set $velocity[0] = 3643.5044963902051
  set $velocity[1] = -4634.4388877243673
  set $velocity[2] = -4001.236670030642
  call ((void (*)(void *, void *, void *, void *, void *, void *, void *, void *, void *, void *)) PosVel2Elements_M)($a, $e, $i, $raan, $argp, $true_anomaly, $mean_anomaly, $argument_of_latitude, $position, $velocity)
  set $out[8]=$a[0]
  set $out[9]=$e[0]
  set $out[10]=$i[0]
  set $out[11]=$raan[0]
  set $out[12]=$argp[0]
  set $out[13]=$true_anomaly[0]
  set $out[14]=$mean_anomaly[0]
  set $out[15]=$argument_of_latitude[0]
  dump binary memory /home/ubuntu/dynamicpackage_audit_tar/dynamicpackage_recovered_work/analysis/time_orbit/posvel2elements_m_gold.bin $out $out+16
  kill
  quit
end
run
