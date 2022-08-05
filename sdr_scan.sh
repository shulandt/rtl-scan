#!/bin/bash

trap "echo stop; exit;" SIGINT SIGTERM

if [ $# -ne 7 ]
then
    echo Usage sdr_scan.sh device min_freq max_freq freq_interval min_bandwidth min_snr output_file
    exit
fi
step=`echo "($3 - $2) * 50" | bc`
echo scanning from $2 MHz to $3 MHz, step $step Hz, freq interval $4 MHz, min bandwidth $5 MHz, min snr $6 dB 
while [ 1 == 1 ]
    do
        echo -n .
        sudo rtl_power -d $1 -g 20 -i 3 -1 -w hamming -f $2M:$3M:$step rtl-power.csv > /dev/null 2>&1
        rtl-scan rtl-power.csv $7 $4 $5 $6
    done
#python3 flatten.py rtl-power.csv > rtl-power-sort.csv