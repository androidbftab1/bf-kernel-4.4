#!/bin/sh

. /sbin/config.sh

if [ "$CONFIG_ANDROID" == "y" ]; then
	IPSEC_CONF_FILE=/data/router/etc/ipsec.conf
else
	IPSEC_CONF_FILE=/etc/ipsec.conf
fi

PLUTO_PID=/var/run/pluto/pluto.pid
	
if [ ! -n "$2" ]; then
       echo "insufficient arguments!"
       echo "Usage: $0 <key_mode:ike/manual> <conn_name>"
       exit 0
fi

KEY_MODE="$1"
CONN_NAME="$2"

#If pluto is not running, just start it
if [ ! -f $PLUTO_PID ]; then
	if [ "$CONFIG_ANDROID" == "y" ]; then
		mkdir /data/router/etc/ipsec.d/
		mkdir /data/router/etc/ipsec.d/cacerts # X.509 root certificates
		mkdir /data/router/etc/ipsec.d/certs # X.509 client certificates
		mkdir /data/router/etc/ipsec.d/private # X.509 private certificates
		mkdir /data/router/etc/ipsec.d/crls # X.509 certificate Revocation lists
		mkdir /data/router/etc/ipsec.d/ocspcerts # X.509 online certificate status protocol certificates
		mkdir /data/router/etc/ipsec.d/passwd #XAUTH password file
		mkdir /data/router/etc/ipsec.d/policies #the opportunistic encryption policy groups
#		mkdir /etc/ipsec.d/aacerts 
	else
		mkdir /etc/ipsec.d/
		mkdir /etc/ipsec.d/cacerts # X.509 root certificates
		mkdir /etc/ipsec.d/certs # X.509 client certificates
		mkdir /etc/ipsec.d/private # X.509 private certificates
		mkdir /etc/ipsec.d/crls # X.509 certificate Revocation lists
		mkdir /etc/ipsec.d/ocspcerts # X.509 online certificate status protocol certificates
		mkdir /etc/ipsec.d/passwd #XAUTH password file
		mkdir /etc/ipsec.d/policies #the opportunistic encryption policy groups
#		mkdir /etc/ipsec.d/aacerts 
	fi
	ipsec setup --start
fi

if [ $KEY_MODE = "ike" ]; then
	ipsec auto --replace $CONN_NAME
#	ipsec auto --up $CONN_NAME
else #Manual
	ipsec manual --up $CONN_NAME
fi

