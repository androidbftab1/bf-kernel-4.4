#!/bin/sh

#=============================================================================
# smp_affinity: 1 = CPU1, 2 = CPU2, 3 = CPU3, 4 = CPU4
# rps_cpus: wxyz = CPU3 CPU2 CPU1 CPU0 (ex:0xd = 0'b1101 = CPU1, CPU3, CPU4)
#=============================================================================

. /sbin/config.sh

dbdc=`nvram_get 2860 DBDC_MODE`
if [ ! -n "$1" ]; then
  echo "insufficient arguments!"
  echo "Usage: $0 <mode>"
  exit 0
fi

OPTIMIZED_FOR="$1"
LIST=`cat /proc/interrupts | sed -n '1p'`
NUM_OF_CPU=0; for i in $LIST; do NUM_OF_CPU=`expr $NUM_OF_CPU + 1`; done;

PHY_POS=`expr $NUM_OF_CPU + 3` #physical irq # position in /proc/interrups may vary with the number of CPU up
DYNAMIC=`cat /proc/version | sed 's/\.//g' | sed 's/\+//g' | awk '$3>=31819 {print "y"}'` # if kernel version >= 3.18.19, use dynamic allocation of irq

#return virtual irq based on physical irq
get_virtual_irq() {
	target_phy_irq=$1
	cat /proc/interrupts | sed 's/:/ /g'| awk '$1 ~ /^[0-9]+$/' | while read line 
	do
		set -- $line
		phy_irq=$(eval "echo \$$PHY_POS")
		if [ $phy_irq == $target_phy_irq ]; then 
			echo $1
			return
		fi
	done
}


if [ $OPTIMIZED_FOR == "wifi" ]; then

	if [ $NUM_OF_CPU == "4" ]; then
		if [ $DYNAMIC = "y" ]; then
			echo 2 > /proc/irq/$(get_virtual_irq 200)/smp_affinity	#GMAC
			echo 1 > /proc/irq/$(get_virtual_irq 199)/smp_affinity  #GMAC
			echo 2 > /proc/irq/$(get_virtual_irq 198)/smp_affinity  #GMAC
           		echo 8 > /proc/irq/$(get_virtual_irq 194)/smp_affinity	#PCIe1
           		echo 4 > /proc/irq/$(get_virtual_irq 195)/smp_affinity	#PCIe2
            		echo 8 > /proc/irq/$(get_virtual_irq 40)/smp_affinity	#SDXC
		        echo 8 > /proc/irq/$(get_virtual_irq 196)/smp_affinity	#USB P0
		        echo 8 > /proc/irq/$(get_virtual_irq 197)/smp_affinity	#USB P1
		        echo 3 > /sys/class/net/ra0/queues/rx-0/rps_cpus

           		if [ -d "/sys/class/net/rae0" ]; then
				echo 1 > /proc/irq/$(get_virtual_irq 193)/smp_affinity	#PCIe0            
		                echo d > /sys/class/net/eth2/queues/rx-0/rps_cpus
		                echo d > /sys/class/net/eth2.2/queues/rx-0/rps_cpus # should change back to eth3 in the future
        		   	echo 6 > /sys/class/net/rai0/queues/rx-0/rps_cpus
				echo a > /sys/class/net/rae0/queues/rx-0/rps_cpus
				echo 3 > /sys/class/net/ra0/queues/rx-0/rps_cpus
		               echo "eth2/eth3 RPS: CPU0/2/3"
        		elif [ -d "/sys/class/net/rai0" ]; then
            			echo 4 > /proc/irq/$(get_virtual_irq 193)/smp_affinity	#PCIe0
		                echo 5 > /sys/class/net/eth2/queues/rx-0/rps_cpus
        		        echo 5 > /sys/class/net/eth2.2/queues/rx-0/rps_cpus # should change back to eth3 in the future
            			echo 3 > /sys/class/net/rai0/queues/rx-0/rps_cpus
				echo 3 > /sys/class/net/rae0/queues/rx-0/rps_cpus
		                echo "eth2/eth3 RPS: CPU0/2"
    			else
            			echo 4 > /proc/irq/$(get_virtual_irq 193)/smp_affinity	#PCIe0
	                	echo 5 > /sys/class/net/eth2/queues/rx-0/rps_cpus
		                echo 5 > /sys/class/net/eth2.2/queues/rx-0/rps_cpus # should change back to eth3 in the future
        		    	echo 3 > /sys/class/net/rai0/queues/rx-0/rps_cpus
				echo 3 > /sys/class/net/rae0/queues/rx-0/rps_cpus
                		echo "eth2/eth3 RPS: CPU0/2"
    	    		fi
		elif [ "$CONFIG_RALINK_MT7621" = "y" ]; then
			echo 2 > /proc/irq/3/smp_affinity  #GMAC
			echo 4 > /proc/irq/4/smp_affinity  #PCIe0
			echo 8 > /proc/irq/24/smp_affinity #PCIe1
			echo 8 > /proc/irq/25/smp_affinity #PCIe2
			echo 8 > /proc/irq/19/smp_affinity #VPN
			echo 8 > /proc/irq/20/smp_affinity #SDXC
			echo 8 > /proc/irq/22/smp_affinity #USB

			echo 3 > /sys/class/net/ra0/queues/rx-0/rps_cpus
			echo 3 > /sys/class/net/rai0/queues/rx-0/rps_cpus
			echo 3 > /sys/class/net/apcli0/queues/rx-0/rps_cpus
			echo 3 > /sys/class/net/apclii0/queues/rx-0/rps_cpus
			echo 3 > /sys/class/net/wds0/queues/rx-0/rps_cpus
			echo 3 > /sys/class/net/wds1/queues/rx-0/rps_cpus
			echo 3 > /sys/class/net/wds2/queues/rx-0/rps_cpus
			echo 3 > /sys/class/net/wds3/queues/rx-0/rps_cpus
			echo 3 > /sys/class/net/wdsi0/queues/rx-0/rps_cpus
			echo 3 > /sys/class/net/wdsi1/queues/rx-0/rps_cpus
			echo 3 > /sys/class/net/wdsi2/queues/rx-0/rps_cpus
			echo 3 > /sys/class/net/wdsi3/queues/rx-0/rps_cpus
			if [ -d "/sys/class/net/rai0" ]; then
                                echo 5 > /sys/class/net/eth2/queues/rx-0/rps_cpus
                                echo 5 > /sys/class/net/eth3/queues/rx-0/rps_cpus
                                echo "eth2/eth3 RPS: CPU0/2"
                        else
                        	if [ "$dbdc" = "1" ]; then
					if [ -d "/sys/class/net/rax0" ]; then
						echo 3 > /sys/class/net/rax0/queues/rx-0/rps_cpus
					else
						echo 3 > /sys/class/net/ra1/queues/rx-0/rps_cpus
					fi

					if [ -d "/sys/class/net/apclix0" ]; then
						echo 3 > /sys/class/net/apclix0/queues/rx-0/rps_cpus
					else
						echo 3 > /sys/class/net/apcli1/queues/rx-0/rps_cpus
					fi
					echo 8 > /proc/irq/4/smp_affinity  #PCIe0
					echo 5 > /sys/class/net/eth2/queues/rx-0/rps_cpus
					echo 5 > /sys/class/net/eth3/queues/rx-0/rps_cpus
					echo "eth2/eth3 RPS: CPU0/2"
				else
					echo 3 > /sys/class/net/eth2/queues/rx-0/rps_cpus
					echo 3 > /sys/class/net/eth3/queues/rx-0/rps_cpus			
					echo "eth2/eth3 RPS: CPU0/1"				
				fi
    			fi
		elif [ "$CONFIG_ARCH_MT7623" = "y" ]; then
			echo 2 > /proc/irq/232/smp_affinity	#GMAC
			echo 1 > /proc/irq/231/smp_affinity     #GMAC
			echo 2 > /proc/irq/230/smp_affinity     #GMAC
           		echo 8 > /proc/irq/226/smp_affinity	#PCIe1
           		echo 4 > /proc/irq/227/smp_affinity	#PCIe2
            		echo 8 > /proc/irq/72/smp_affinity	#SDXC
            		echo 8 > /proc/irq/228/smp_affinity	#USB P0
            		echo 8 > /proc/irq/229/smp_affinity	#USB P1
            		echo 3 > /sys/class/net/ra0/queues/rx-0/rps_cpus

	  	        if [ -d "/sys/class/net/rae0" ]; then
				echo 1 > /proc/irq/225/smp_affinity	#PCIe0            
                		echo d > /sys/class/net/eth2/queues/rx-0/rps_cpus
	                	echo d > /sys/class/net/eth3/queues/rx-0/rps_cpus
        	    		echo 6 > /sys/class/net/rai0/queues/rx-0/rps_cpus
				echo a > /sys/class/net/rae0/queues/rx-0/rps_cpus
                		echo "eth2/eth3 RPS: CPU0/2/3"
		        elif [ -d "/sys/class/net/rai0" ]; then
            			echo 4 > /proc/irq/225/smp_affinity	#PCIe0
                		echo 5 > /sys/class/net/eth2/queues/rx-0/rps_cpus
	                	echo 5 > /sys/class/net/eth3/queues/rx-0/rps_cpus
        			echo 3 > /sys/class/net/rai0/queues/rx-0/rps_cpus
				echo 3 > /sys/class/net/rae0/queues/rx-0/rps_cpus
                		echo "eth2/eth3 RPS: CPU0/2"
			else
            			echo 4 > /proc/irq/225/smp_affinity	#PCIe0
	                	echo 5 > /sys/class/net/eth2/queues/rx-0/rps_cpus
        	        	echo 5 > /sys/class/net/eth3/queues/rx-0/rps_cpus
            			echo 3 > /sys/class/net/rai0/queues/rx-0/rps_cpus
				echo 3 > /sys/class/net/rae0/queues/rx-0/rps_cpus
	                	echo "eth2/eth3 RPS: CPU0/2"
    			fi
		fi

	elif [ $NUM_OF_CPU == "2" ]; then
		if [ $DYNAMIC = "y" ]; then
			echo 2 > /proc/irq/$(get_virtual_irq 200)/smp_affinity	#GMAC
			echo 1 > /proc/irq/$(get_virtual_irq 193)/smp_affinity	#PCIe0
                	echo 2 > /proc/irq/$(get_virtual_irq 194)/smp_affinity	#PCIe1
                	echo 2 > /proc/irq/$(get_virtual_irq 195)/smp_affinity	#PCIe2
                	echo 1 > /proc/irq/$(get_virtual_irq 40)/smp_affinity	#SDXC
			echo 1 > /proc/irq/$(get_virtual_irq 196)/smp_affinity	#USB P0
			echo 1 > /proc/irq/$(get_virtual_irq 197)/smp_affinity	#USB P1

                	echo 2 > /sys/class/net/ra0/queues/rx-0/rps_cpus
                	echo 1 > /sys/class/net/rai0/queues/rx-0/rps_cpus
                	echo 2 > /sys/class/net/eth2/queues/rx-0/rps_cpus
                	echo 2 > /sys/class/net/eth2.2/queues/rx-0/rps_cpus # should change back to eth3 in the future
		elif [ "$CONFIG_RALINK_MT7621" = "y" ]; then
			echo 2 > /proc/irq/3/smp_affinity  #GMAC
			echo 1 > /proc/irq/4/smp_affinity  #PCIe0
			echo 2 > /proc/irq/24/smp_affinity #PCIe1
			echo 2 > /proc/irq/25/smp_affinity #PCIe2
			echo 1 > /proc/irq/19/smp_affinity #VPN
			echo 1 > /proc/irq/20/smp_affinity #SDXC
			echo 1 > /proc/irq/22/smp_affinity #USB

			echo 2 > /sys/class/net/ra0/queues/rx-0/rps_cpus
			echo 1 > /sys/class/net/rai0/queues/rx-0/rps_cpus
			echo 2 > /sys/class/net/eth2/queues/rx-0/rps_cpus
			echo 2 > /sys/class/net/eth3/queues/rx-0/rps_cpus
		elif [ "$CONFIG_ARCH_MT7623" = "y" ]; then
			echo 2 > /proc/irq/232/smp_affinity	#GMAC
			echo 1 > /proc/irq/225/smp_affinity	#PCIe0
                	echo 2 > /proc/irq/226/smp_affinity	#PCIe1
                	echo 2 > /proc/irq/227/smp_affinity	#PCIe2
                	echo 1 > /proc/irq/72/smp_affinity	#SDXC
			echo 1 > /proc/irq/228/smp_affinity	#USB P0
			echo 1 > /proc/irq/229/smp_affinity	#USB P1

                	echo 2 > /sys/class/net/ra0/queues/rx-0/rps_cpus
                	echo 1 > /sys/class/net/rai0/queues/rx-0/rps_cpus
                	echo 2 > /sys/class/net/eth2/queues/rx-0/rps_cpus
                	echo 2 > /sys/class/net/eth3/queues/rx-0/rps_cpus
		fi

	fi

elif [ $OPTIMIZED_FOR == "storage" ]; then

	if [ $NUM_OF_CPU == "4" ]; then
		if [ $DYNAMIC = "y" ]; then
			echo 4 > /proc/irq/$(get_virtual_irq 196)/smp_affinity 	#USB
			echo 4 > /proc/irq/$(get_virtual_irq 197)/smp_affinity 	#USB
			echo 2 > /proc/irq/$(get_virtual_irq 198)/smp_affinity  #GMAC
			echo 1 > /proc/irq/$(get_virtual_irq 199)/smp_affinity  #GMAC
			echo 2 > /proc/irq/$(get_virtual_irq 200)/smp_affinity	#GMAC
                        echo 4 > /proc/irq/$(get_virtual_irq 193)/smp_affinity	#PCIe0
                        echo 4 > /proc/irq/$(get_virtual_irq 194)/smp_affinity	#PCIe1
                        echo 4 > /proc/irq/$(get_virtual_irq 195)/smp_affinity	#PCIe2
                        echo 4 > /proc/irq/$(get_virtual_irq 40)/smp_affinity	#SDXC
		elif [ "$CONFIG_RALINK_MT7621" = "y" ]; then
			echo 1 > /proc/irq/3/smp_affinity  #GMAC Tx/Rx
			echo 2 > /proc/irq/4/smp_affinity  #PCIe0
			echo 2 > /proc/irq/24/smp_affinity #PCIe1
			echo 2 > /proc/irq/25/smp_affinity #PCIe2
			echo 4 > /proc/irq/19/smp_affinity #VPN
			echo 4 > /proc/irq/20/smp_affinity #SDXC
			echo 4 > /proc/irq/22/smp_affinity #USB
		elif [ "$CONFIG_ARCH_MT7623" = "y" ]; then
			echo 1 > /proc/irq/231/smp_affinity     #GMAC Tx
			echo 2 > /proc/irq/230/smp_affinity     #GMAC Rx
			echo 2 > /proc/irq/232/smp_affinity	#GMAC Tx/Rx
       			echo 4 > /proc/irq/228/smp_affinity 	#USB P0
			echo 4 > /proc/irq/229/smp_affinity 	#USB P1
			echo 4 > /proc/irq/225/smp_affinity	#PCIe0
                        echo 4 > /proc/irq/226/smp_affinity	#PCIe1
                        echo 4 > /proc/irq/227/smp_affinity	#PCIe2
                        echo 4 > /proc/irq/72/smp_affinity	#SDXC
		fi

		echo 2 > /sys/class/net/ra0/queues/rx-0/rps_cpus
		echo 2 > /sys/class/net/rai0/queues/rx-0/rps_cpus
		echo 2 > /sys/class/net/eth2/queues/rx-0/rps_cpus
		echo 2 > /sys/class/net/eth2.2/queues/rx-0/rps_cpus # should change back to eth3 in the future
	elif [ $NUM_OF_CPU == "2" ]; then
		if [ $DYNAMIC = "y" ]; then
			echo 1 > /proc/irq/$(get_virtual_irq 196)/smp_affinity 	#USB
			echo 1 > /proc/irq/$(get_virtual_irq 197)/smp_affinity 	#USB
			echo 1 > /proc/irq/$(get_virtual_irq 198)/smp_affinity  #GMAC
			echo 1 > /proc/irq/$(get_virtual_irq 199)/smp_affinity  #GMAC
			echo 1 > /proc/irq/$(get_virtual_irq 200)/smp_affinity	#GMAC
                        echo 1 > /proc/irq/$(get_virtual_irq 193)/smp_affinity	#PCIe0
                        echo 1 > /proc/irq/$(get_virtual_irq 194)/smp_affinity	#PCIe1
                        echo 1 > /proc/irq/$(get_virtual_irq 195)/smp_affinity	#PCIe2
                        echo 1 > /proc/irq/$(get_virtual_irq 40)/smp_affinity	#SDXC
		elif [ "$CONFIG_RALINK_MT7621" = "y" ]; then
			echo 1 > /proc/irq/3/smp_affinity  #GMAC
			echo 1 > /proc/irq/4/smp_affinity  #PCIe0
			echo 1 > /proc/irq/24/smp_affinity #PCIe1
			echo 1 > /proc/irq/25/smp_affinity #PCIe2
			echo 1 > /proc/irq/19/smp_affinity #VPN
			echo 1 > /proc/irq/20/smp_affinity #SDXC
			echo 1 > /proc/irq/22/smp_affinity #USB
		elif [ "$CONFIG_ARCH_MT7623" = "y" ]; then
			echo 1 > /proc/irq/228/smp_affinity 	#USB
			echo 1 > /proc/irq/229/smp_affinity 	#USB
			echo 1 > /proc/irq/230/smp_affinity     #GMAC
			echo 1 > /proc/irq/231/smp_affinity     #GMAC
			echo 1 > /proc/irq/232/smp_affinity	#GMAC
                        echo 1 > /proc/irq/225/smp_affinity	#PCIe0
                        echo 1 > /proc/irq/226/smp_affinity	#PCIe1
                        echo 1 > /proc/irq/227/smp_affinity	#PCIe2
                        echo 1 > /proc/irq/72/smp_affinity	#SDXC
		fi

		echo 1 > /sys/class/net/ra0/queues/rx-0/rps_cpus
		echo 1 > /sys/class/net/rai0/queues/rx-0/rps_cpus
		echo 1 > /sys/class/net/eth2/queues/rx-0/rps_cpus
		echo 1 > /sys/class/net/eth2.2/queues/rx-0/rps_cpus # should change back to eth3 in the future


	fi

else

	echo "unknow arguments!"
	exit 0

fi
