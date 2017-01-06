#!/bin/sh
#
# $Id$
#
# usage: nat.sh
#

. /sbin/config.sh
. /sbin/global.sh


lan_ip=`nvram_get 2860 lan_ipaddr`
nat_en=`nvram_get 2860 natEnabled`
tcp_timeout=`nvram_get 2860 TcpTimeout`
udp_timeout=`nvram_get 2860 UdpTimeout`
conntrack_max=`nvram_get 2860 MaxConntrack`

if [ "$nat_en" = "1" ]; then
	if [ "$CONFIG_NF_CONNTRACK_SUPPORT" != "" -o "$CONFIG_NF_CONNTRACK" != "" ]; then
		if [ "$udp_timeout" = "" ]; then
			echo 180 > /proc/sys/net/netfilter/nf_conntrack_udp_timeout
		else
			echo "$udp_timeout" > /proc/sys/net/netfilter/nf_conntrack_udp_timeout
		fi

		if [ "$tcp_timeout" = "" ]; then
			echo 1800 >  /proc/sys/net/netfilter/nf_conntrack_tcp_timeout_established
		else
			echo "$tcp_timeout" >  /proc/sys/net/netfilter/nf_conntrack_tcp_timeout_established
		fi

		if [ "$conntrack_max" = "" ]; then
			nf_conntrack_max=`cat /proc/sys/net/netfilter/nf_conntrack_max`
			if [ $nf_conntrack_max -lt 8000 ]; then
				echo 8000 > /proc/sys/net/netfilter/nf_conntrack_max
			fi
		else
			echo $conntrack_max > /proc/sys/net/netfilter/nf_conntrack_max
		fi
	else
		if [ "$udp_timeout" = "" ]; then
			echo 180 > /proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout
		else
			echo "$udp_timeout" > /proc/sys/net/ipv4/netfilter/ip_conntrack_udp_timeout
		fi

		if [ "$tcp_timeout" = "" ]; then
			echo 1800 > /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established
		else
			echo "$tcp_timeout" > /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_timeout_established
		fi

		if [ "$conntrack_max" = "" ]; then
			ip_conntrack_max=`cat /proc/sys/net/netfilter/ip_conntrack_max`
			if [ $ip_conntrack_max -lt 8000 ]; then
				echo 8000 > /proc/sys/net/netfilter/ip_conntrack_max
			fi
		else
			echo $conntrack_max > /proc/sys/net/netfilter/ip_conntrack_max
		fi
	fi
	if [ "$wanmode" = "PPPOE" -o "$wanmode" = "L2TP" -o "$wanmode" = "PPTP" -o "$wanmode" = "3G" ]; then
		wan_if="ppp0"
	fi
	iptables -t nat -A POSTROUTING -s $lan_ip/24 -o $wan_if -j MASQUERADE
fi

