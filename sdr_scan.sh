#!/bin/bash

trap "echo stop; exit;" SIGINT SIGTERM

source ./sdr_scan.conf

STEP=`echo "($END_FREQ - $BEGIN_FREQ) * 50" | bc`
echo scanning from $BEGIN_FREQ MHz to $END_FREQ MHz, step $STEP Hz, freq interval $INTERVAL_FREQ MHz, min bandwidth $MIN_BANDWIDTH MHz, min snr $MIN_SNR dB 
while [ 1 == 1 ]
    do
        echo -n .
        sudo rtl_power -d $DEVICE -g 20 -i 3 -1 -w hamming -f ${BEGIN_FREQ}M:${END_FREQ}M:$STEP rtl-power.csv > /dev/null 2>&1
        rtl-scan rtl-power.csv $OUT_FILE $INTERVAL_FREQ $MIN_BANDWIDTH $MIN_SNR
    done
#python3 flatten.py rtl-power.csv > rtl-power-sort.csv