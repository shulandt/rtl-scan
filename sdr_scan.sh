#!/bin/bash

trap "echo stop; exit;" SIGINT SIGTERM

LOG_PATH=/home/pi/rtl-scan

source $LOG_PATH/sdr_scan.conf

while [ 1 == 1 ]
do
    for (( RANGE = 1; RANGE <= RANGE_QUANT; RANGE++ ))
    do
        BEGIN_FREQ=BEGIN_FREQ_$(echo $RANGE)
        BEGIN_FREQ=${!BEGIN_FREQ}
        END_FREQ=END_FREQ_$(echo $RANGE)
        END_FREQ=${!END_FREQ}
        INTERVAL_FREQ=INTERVAL_FREQ_$(echo $RANGE)
        INTERVAL_FREQ=${!INTERVAL_FREQ}
        MIN_BANDWIDTH=MIN_BANDWIDTH_$(echo $RANGE)
        MIN_BANDWIDTH=${!MIN_BANDWIDTH}
        MIN_SNR=MIN_SNR_$(echo $RANGE)
        MIN_SNR=${!MIN_SNR}

        STEP=`echo "($END_FREQ - $BEGIN_FREQ) * 50" | bc`
        echo -e -n "\rscanning from $BEGIN_FREQ MHz to $END_FREQ MHz, step $STEP Hz..."
        sudo rtl_power -d $DEVICE -g 20 -i 3 -1 -w hamming -f ${BEGIN_FREQ}M:${END_FREQ}M:$STEP /tmp/rtl-power.csv > /dev/null 2>&1
        sudo rtl-scan /tmp/rtl-power.csv $LOG_PATH/$OUT_FILE $INTERVAL_FREQ $MIN_BANDWIDTH $MIN_SNR
    done
done

#python3 flatten.py rtl-power.csv > rtl-power-sort.csv