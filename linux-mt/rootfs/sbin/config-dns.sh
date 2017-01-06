#!/bin/sh

# $Id$
# usage: config-dns.sh [<dns1>] [<dns2>]

. /sbin/config.sh

if [ "$CONFIG_ANDROID" == "y" ]; then
	fname="/data/router/etc/resolv.conf"
	fbak="/data/router/etc/resolv_conf.bak"
else
	fname="/etc/resolv.conf"
	fbak="/etc/resolv_conf.bak"
fi

# in case no previous file
touch $fname

# backup file without nameserver part
sed -e '/nameserver/d' $fname > $fbak

# set primary and seconday DNS
if [ "$1" != "" ]; then
  echo "nameserver $1" > $fname
else # empty dns
  rm -f $fname
fi
if [ "$2" != "" ]; then
  echo "nameserver $2" >> $fname
fi

cat $fbak >> $fname
rm -f $fbak

