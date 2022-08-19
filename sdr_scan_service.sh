#!/bin/sh

### BEGIN INIT INFO
# Provides:          sdr_scan
# Required-Start:    $network $local_fs
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: sdr scanner
# Description:       sdr scanner
### END INIT INFO

/home/ubuntu/rtl-scan/sdr_scan.sh &
