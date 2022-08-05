#!/bin/bash

trap "echo stop; exit;" SIGINT SIGTERM

#SDR device number
DEVICE=0
#Begin frequency, MHz
BEGIN_FREQ=462.0
#End frequency, MHz
END_FREQ=463.8
#Min frequency interval between signals, kHz
INTERVAL_FREQ=100.0
#Min bandwidth for signal, kHz
MIN_BANDWIDTH=1.0
#Min SNR for signal, dB
MIN_SNR=10
#Output file name
OUT_FILE=freq_scan.csv

STEP=`echo "($END_FREQ - $BEGIN_FREQ) * 50" | bc`
echo scanning from $BEGIN_FREQ MHz to $END_FREQ MHz, step $STEP Hz, freq interval $INTERVAL_FREQ kHz, min bandwidth $MIN_BANDWIDTH kHz, min snr $MIN_SNR dB 
while [ 1 == 1 ]
    do
        echo -n .
        sudo rtl_power -d $DEVICE -g 20 -i 3 -1 -w hamming -f $BEGIN_FREQM:$END_FREQM:$step rtl-power.csv > /dev/null 2>&1
        rtl-scan rtl-power.csv $OUT_FILE $INTERVAL_FREQ $MIN_BANDWIDTH $MIN_SNR
    done
#python3 flatten.py rtl-power.csv > rtl-power-sort.csv