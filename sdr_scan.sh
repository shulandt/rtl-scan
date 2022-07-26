#!/bin/bash

trap "echo stop; exit;" SIGINT SIGTERM

if [ $# -ne 3 ]
then
    echo Usage sdr_scan.sh min_freq max_freq output_file
    exit
fi
step=`echo "($2 - $1) * 50" | bc`
echo scanning from $1 MHz to $2 MHz, step $step Hz ...
#while [ 1 == 1 ]
#    do
        sudo rtl_power -g 0 -i 3 -1 -w hamming -f $1M:$2M:$step rtl-power.csv > /dev/null 2>&1
        rtl-scan rtl-power.csv $3
#    done
#python3 flatten.py rtl-power.csv > rtl-power-sort.csv