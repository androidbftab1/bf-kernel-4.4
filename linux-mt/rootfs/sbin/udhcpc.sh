#!/bin/sh

# udhcpc script edited by Tim Riker <Tim@Rikers.org>

. /sbin/config.sh
. /sbin/global.sh

[ -z "$1" ] && echo "Error: should be called from udhcpc" && exit 1

if [ "$CONFIG_ANDROID" == "y" ]; then
RESOLV_CONF="/data/router/etc/resolv.conf"
PPP_CONF="/data/router/etc/options.pptp"
IFCONFIG=/bin/ifconfig
wan_status_file=/var/wan_status
wan_settings_file=/var/wan_settings
else
RESOLV_CONF="/etc/resolv.conf"
PPP_CONF="/etc/options.pptp"
IFCONFIG=/sbin/ifconfig
fi

[ -n "$broadcast" ] && BROADCAST="broadcast $broadcast"
[ -n "$subnet" ] && NETMASK="netmask $subnet"

case "$1" in
    deconfig)
        $IFCONFIG $interface 0.0.0.0
        if [ "$CONFIG_ANDROID" == "y" ]; then
                echo "wan_done=0" > $wan_status_file
	fi
        ;;

    renew|bound)
        $IFCONFIG $interface $ip $BROADCAST $NETMASK

        if [ "$CONFIG_ANDROID" == "y" ]; then
		sed -i "/wan_interface/s/=.*$/=$interface/g" $wan_settings_file
		sed -i "/wan_ipaddr/s/=.*$/=$ip/g"           $wan_settings_file
		sed -i "/wan_netmask/s/=.*$/=$subnet/g"      $wan_settings_file
		sed -i "/wan_gateway/s/=.*$/=$router/g"      $wan_settings_file
        fi

	#/* remove all binding entries after getting new IP */
	HWNAT=`nvram_get 2860 hwnatEnabled`
	if [ "$HWNAT" = "1" ]; then
		rmmod hw_nat
        	insmod -q hw_nat
        	if [ "$CONFIG_RA_HW_NAT_WIFI_NEW_ARCH" = "y" ]; then
		iwpriv ra0 set hw_nat_register=1
			if [ "$CONFIG_SECOND_IF_MT7615E" = "y" ]; then
			iwpriv rai0 set hw_nat_register=1
			fi
			if [ "$CONFIG_THIRD_IF_MT7615E" = "y" ]; then
			iwpriv rae0 set hw_nat_register=1
			fi
		fi
	fi

        if [ -n "$router" ] ; then
            echo "deleting routers"
            while route del default gw 0.0.0.0 dev $interface ; do
                :
            done

            metric=0
            for i in $router ; do
                metric=`expr $metric + 1`
                route add default gw $i dev $interface metric $metric
            done
        fi

        echo -n > $RESOLV_CONF
        [ -n "$domain" ] && echo search $domain >> $RESOLV_CONF

        if [ "$CONFIG_ANDROID" == "y" ]; then
            dhcpcDnsPri=
            dhcpcDnsSec=
            for i in $dns ; do
                echo adding dns $i
                echo nameserver $i >> $RESOLV_CONF
                if [ -z $dhcpcDnsPri ]; then
                    dhcpcDnsPri=$i
                elif [ -z $dhcpcDnsSec ]; then
                    dhcpcDnsSec=$i
                fi
            done
            sed -i "/wan_primary_dns/s/=.*$/=$dhcpcDnsPri/g"   $wan_settings_file
            sed -i "/wan_secondary_dns/s/=.*$/=$dhcpcDnsSec/g" $wan_settings_file
        else
            for i in $dns ; do
                echo adding dns $i
                echo nameserver $i >> $RESOLV_CONF
            done
        fi

		# notify goahead when the WAN IP has been acquired. --yy
		killall -SIGTSTP goahead
		killall -SIGTSTP nvram_daemon

		# restart igmpproxy daemon (nvram_daemon handle igmpproxy)
		# config-igmpproxy.sh

		if [ "$wanmode" = "L2TP" ]; then
			if [ "$CONFIG_PPPOL2TP" == "y" ]; then
				killall openl2tpd
			        rm -f /var/run/openl2tpd.pid
                                if [ "$CONFIG_ANDROID" == "y" ]; then
			                openl2tpd -u 1701 -c /data/router/etc/openl2tpd.conf
			        else
			                openl2tpd
			        fi
			else
				killall l2tpd
				l2tpd
				sleep 1
				l2tp-control "start-session $l2tp_srv"
			fi
		elif [ "$wanmode" = "PPTP" ]; then
			killall pppd
			pppd file $PPP_CONF &
                else
                        if [ "$CONFIG_ANDROID" == "y" ]; then
			    sed -i "/wanConnectionMode/s/=.*$/=DHCP/g" $wan_settings_file
		            echo "wan_done=1" > $wan_status_file
                        fi
		fi
        ;;
esac
#echo 'end of udhcpc.sh'
exit 0

