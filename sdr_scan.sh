#!/bin/bash

trap "echo stop; exit;" SIGINT SIGTERM

echo scanning...
while [ 1 == 1 ]
    do
        sudo rtl_power -g 0 -i 3 -1 -w hamming -f 461M:464M:100 rtl-power.csv > /dev/null 2>&1
        rtl-scan rtl-power.csv
    done
#python3 flatten.py rtl_power.csv > rtl_power_sort.csv