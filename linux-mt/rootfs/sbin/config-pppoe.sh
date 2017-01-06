#!/bin/sh
#
# $Id$
#
# usage: config-pppoe.sh <user> <password> <wan_if_name>
#

. /sbin/config.sh

usage()
{
	echo "Usage:"
	echo "  $0 <user> <password> <wan_if_name>"
	exit 1
}

if [ "$3" = "" ]; then
	echo "$0: insufficient arguments"
	usage $0
fi


syslogd -m 0
pppoe.sh $1 $2 $3 $4 $5

if [ "$CONFIG_ANDROID" == "y" ]; then
	pppd file /data/router/etc/options.pppoe &
else
	pppd file /etc/options.pppoe &	
fi

iptables -t MANGLE -A FORWARD -p tcp --tcp-flags SYN,RST SYN -j TCPMSS --clamp-mss-to-pmtu


