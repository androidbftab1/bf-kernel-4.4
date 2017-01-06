#!/bin/sh
#
# $Id$
#
# usage: wan.sh
#

. /sbin/global.sh


# $1: mac address of wan interface.
AddSwitchMacTable()
{
	mac_addr=`echo $1 | sed 's/://g'`
	echo "Set static mac entry of wan mac addr to mac table."
	if [ "$CONFIG_RALINK_MT7620" = "y" ]; then
		# MT7620 use 1 CPU Port = Port6
		switch add $mac_addr 00000010 2
	elif [ "$CONFIG_RALINK_MT7628" = "y" ]; then
		# MT7628 use 1 CPU Port = Port6
		switch add $mac_addr 0000001 2
	elif [ "$CONFIG_RALINK_MT7621" = "y" -o "$CONFIG_ARCH_MT7623" = "y" ]; then
		if [ "$CONFIG_GE2_RGMII_AN" = "y" -o "$CONFIG_GE2_INTERNAL_GPHY" = "y" ]; then
			# WAN CPU Port = Port5.
			switch add $mac_addr 00000100 2
		else
			# The same as MT7620, use 1 CPU Port = Port6
			switch add $mac_addr 00000010 2
		fi
	elif [ "$CONFIG_P5_RGMII_TO_MT7530_MODE" = "y" ]; then
		# MT7620 + MT7530
		if [ "$CONFIG_GE_RGMII_MT7530_P0_AN" = "y" -o "$CONFIG_GE_RGMII_MT7530_P4_AN" = "y" ]; then
		# MT7530 WAN CPU Port = Port5
			switch add $mac_addr 00000100 2
		else
			# The same as MT7620, MT7530 use 1 CPU Port = Port6
			# Should not happen!
			switch add $mac_addr 00000010 2
		fi
	else
		echo "should not happen!"
	fi
}

# stop all
killall -q syslogd
killall -q udhcpc
killall -q pppd
killall -q l2tpd
killall -q openl2tpd
if [ "$CONFIG_NET_VENDOR_MEDIATEK" = "y" -o "$CONFIG_NET_MEDIATEK_SOC" = "y" ]; then
	killall -q wan_statusd
fi

if [ "$CONFIG_ANDROID" == "y" ]; then
	wan_status_file=/var/wan_status
	wan_settings_file=/var/wan_settings
	echo "wan_done=0"         > $wan_status_file
	echo "wanConnectionMode=" > $wan_settings_file
	echo "wan_interface="    >> $wan_settings_file
	echo "wan_ipaddr="       >> $wan_settings_file
	echo "wan_netmask="      >> $wan_settings_file
	echo "wan_gateway="      >> $wan_settings_file
	echo "wan_primary_dns="  >> $wan_settings_file
	echo "wan_secondary_dns=">> $wan_settings_file
fi

clone_en=`nvram_get 2860 macCloneEnabled`
clone_mac=`nvram_get 2860 macCloneMac`
#MAC Clone: bridge mode doesn't support MAC Clone
if [ "$opmode" != "0" -a "$clone_en" = "1" ]; then
	ifconfig $wan_if down
	if [ "$opmode" = "2" ]; then
		if [ "$CONFIG_RT2860V2_STA" == "m" ]; then
		rmmod rt2860v2_sta_net
		rmmod rt2860v2_sta
		rmmod rt2860v2_sta_util

		insmod -q rt2860v2_sta_util
		insmod -q rt2860v2_sta mac=$clone_mac
		insmod -q rt2860v2_sta_net
		elif [ "$CONFIG_RLT_STA_SUPPORT" == "m" ]; then
			rmmod rlt_wifi_sta
			insmod -q rlt_wifi_sta mac=$clone_mac
		fi
	else
		ifconfig $wan_if hw ether $clone_mac
	fi
	ifconfig $wan_if up
fi
wan_mac=`cat /sys/class/net/$wan_if/address`
AddSwitchMacTable $wan_mac

if [ "$wanmode" = "STATIC" -o "$opmode" = "0" ]; then
	#always treat bridge mode having static wan connection
	ip=`nvram_get 2860 wan_ipaddr`
	nm=`nvram_get 2860 wan_netmask`
	gw=`nvram_get 2860 wan_gateway`
	pd=`nvram_get 2860 wan_primary_dns`
	sd=`nvram_get 2860 wan_secondary_dns`

	#lan and wan ip should not be the same except in bridge mode
	if [ "$opmode" != "0" ]; then
		lan_ip=`nvram_get 2860 lan_ipaddr`
		if [ "$ip" = "$lan_ip" ]; then
			echo "wan.sh: warning: WAN's IP address is set identical to LAN"
			exit 0
		fi
	else
		#use lan's ip address instead
		ip=`nvram_get 2860 lan_ipaddr`
		nm=`nvram_get 2860 lan_netmask`
	fi
	ifconfig $wan_if $ip netmask $nm
	route del default
	if [ "$gw" != "" ]; then
	route add default gw $gw
	fi
	config-dns.sh $pd $sd

	if [ "$CONFIG_ANDROID" == "y" ]; then
	        echo "wanConnectionMode=STATIC" > $wan_settings_file
		echo "wan_interface=$wan_if"   >> $wan_settings_file
		echo "wan_ipaddr=$ip"          >> $wan_settings_file
		echo "wan_netmask=$nm"         >> $wan_settings_file
		echo "wan_gateway=$gw"         >> $wan_settings_file
		echo "wan_primary_dns=$pd"     >> $wan_settings_file
		echo "wan_secondary_dns=$sd"   >> $wan_settings_file
		echo "wan_done=1"               > $wan_status_file
	fi
elif [ "$wanmode" = "DHCP" ]; then
	hn=`nvram_get 2860 wan_dhcp_hn`
	if [ "$hn" != "" ]; then
		udhcpc -i $wan_if -h $hn -s /sbin/udhcpc.sh -p /var/run/udhcpc.pid &
	else
		udhcpc -i $wan_if -s /sbin/udhcpc.sh -p /var/run/udhcpc.pid &
	fi
elif [ "$wanmode" = "PPPOE" ]; then
	u=`nvram_get 2860 wan_pppoe_user`
	pw=`nvram_get 2860 wan_pppoe_pass`
	pppoe_opmode=`nvram_get 2860 wan_pppoe_opmode`
	if [ "$pppoe_opmode" = "" ]; then
		echo "pppoecd $wan_if -u $u -p $pw"
		pppoecd $wan_if -u "$u" -p "$pw"
	else
		pppoe_optime=`nvram_get 2860 wan_pppoe_optime`
		config-pppoe.sh $u $pw $wan_if $pppoe_opmode $pppoe_optime
	fi
elif [ "$wanmode" = "L2TP" ]; then
	srv=`nvram_get 2860 wan_l2tp_server`
	u=`nvram_get 2860 wan_l2tp_user`
	pw=`nvram_get 2860 wan_l2tp_pass`
	mode=`nvram_get 2860 wan_l2tp_mode`
	l2tp_opmode=`nvram_get 2860 wan_l2tp_opmode`
	l2tp_optime=`nvram_get 2860 wan_l2tp_optime`
	if [ "$mode" = "0" ]; then
		ip=`nvram_get 2860 wan_l2tp_ip`
		nm=`nvram_get 2860 wan_l2tp_netmask`
		gw=`nvram_get 2860 wan_l2tp_gateway`
		if [ "$gw" = "" ]; then
			gw="0.0.0.0"
		fi
		config-l2tp.sh static $wan_if $ip $nm $gw $srv $u $pw $l2tp_opmode $l2tp_optime
	else
		config-l2tp.sh dhcp $wan_if $srv $u $pw $l2tp_opmode $l2tp_optime
	fi
elif [ "$wanmode" = "PPTP" ]; then
	srv=`nvram_get 2860 wan_pptp_server`
	u=`nvram_get 2860 wan_pptp_user`
	pw=`nvram_get 2860 wan_pptp_pass`
	mode=`nvram_get 2860 wan_pptp_mode`
	pptp_opmode=`nvram_get 2860 wan_pptp_opmode`
	pptp_optime=`nvram_get 2860 wan_pptp_optime`
	if [ "$mode" = "0" ]; then
		ip=`nvram_get 2860 wan_pptp_ip`
		nm=`nvram_get 2860 wan_pptp_netmask`
		gw=`nvram_get 2860 wan_pptp_gateway`
		if [ "$gw" = "" ]; then
			gw="0.0.0.0"
		fi
		config-pptp.sh static $wan_if $ip $nm $gw $srv $u $pw $pptp_opmode $pptp_optime
	else
		config-pptp.sh dhcp $wan_if $srv $u $pw $pptp_opmode $pptp_optime
	fi
elif [ "$wanmode" = "3G" ]; then
	autoconn3G.sh connect &
else
	echo "wan.sh: unknown wan connection type: $wanmode"
	exit 1
fi

# Trigger udhcpc to get IP when wan link up again.
if [ "$CONFIG_NET_MEDIATEK_SOC" = "y" ]; then
	#wan_statusd eth1 &
	wan_statusd $wan_if &
elif [ "$CONFIG_NET_VENDOR_MEDIATEK" = "y" ]; then
	#wan_statusd eth2.2 &
	wan_statusd $wan_if &
fi

