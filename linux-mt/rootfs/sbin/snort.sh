#!/bin/sh
#
# $Id$
#
# usage: snort.sh
#

se=`nvram_get 2860 SnortEnable`

killall -q snort

# debug
#echo "se=$se"

#run snort
if [ "$se" = "1" ]; then
/bin/snort -c /etc_ro/snort.conf -l /var/log -s &
fi
