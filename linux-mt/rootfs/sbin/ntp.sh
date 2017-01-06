#!/bin/sh
#
# $Id$
#
# usage: ntp.sh
#

. /sbin/config.sh

srv=`nvram_get 2860 NTPServerIP`
sync=`nvram_get 2860 NTPSync`
tz=`nvram_get 2860 TZ`


killall -q ntpclient

if [ "$srv" = "" ]; then
	exit 0
fi


#if [ "$sync" = "" ]; then
#	sync=1
#elif [ $sync -lt 300 -o $sync -le 0 ]; then
#	sync=1
#fi

sync=`expr $sync \* 3600`

if [ "$tz" = "" ]; then
	tz="UCT_000"
fi

#debug
#echo "serv=$srv"
#echo "sync=$sync"
#echo "tz=$tz"

if [ "$CONFIG_ANDROID" == "y" ]; then
	echo $tz > /data/router/etc/tmpTZ
	sed -e 's#.*_\(-*\)0*\(.*\)#GMT-\1\2#' /data/router/etc/tmpTZ > /data/router/etc/tmpTZ2
	sed -e 's#\(.*\)--\(.*\)#\1\2#' /data/router/etc/tmpTZ2 > /data/router/etc/TZ
	rm -rf /data/router/etc/tmpTZ
	rm -rf /data/router/etc/tmpTZ2
else
	echo $tz > /etc/tmpTZ
	sed -e 's#.*_\(-*\)0*\(.*\)#GMT-\1\2#' /etc/tmpTZ > /etc/tmpTZ2
	sed -e 's#\(.*\)--\(.*\)#\1\2#' /etc/tmpTZ2 > /etc/TZ
	rm -rf /etc/tmpTZ
	rm -rf /etc/tmpTZ2
fi

ntpclient -s -c 0 -h $srv -i $sync &

