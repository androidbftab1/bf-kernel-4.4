#!/bin/sh
#
# $Id$
#
# usage: config-igmpproxy.sh <wan_if_name> <lan_if_name>
#

. /sbin/global.sh

if [ "$CONFIG_ANDROID" == "y" ]; then 
CONF_FILE=/data/router/etc/igmpproxy.conf
else
CONF_FILE=/etc/igmpproxy.conf
fi

killall -q igmpproxy

igmpEn=`nvram_get 2860 igmpEnabled`
if [ "$igmpEn" == "1" ]; then
igmpproxy.sh $wan_if $lan_if
#echo igmpproxy.conf is in $CONF_FILE
igmpproxy -c $CONF_FILE  -f 1
fi

