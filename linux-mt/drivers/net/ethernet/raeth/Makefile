obj-$(CONFIG_RAETH) += raeth.o
raeth-objs := raether.o raether_pdma.o ra_mac.o mii_mgr.o ra_switch.o ra_dbg_proc.o

ifeq ($(CONFIG_RAETH_QDMA),y)
raeth-objs += raether_qdma.o
endif

ifeq ($(CONFIG_RAETH_ETHTOOL),y)
raeth-objs += ra_ethtool.o
endif

ifeq ($(CONFIG_RAETH_HW_LRO),y)
raeth-objs += raether_hwlro.o
raeth-objs += ra_dbg_hwlro.o
endif

ifeq ($(CONFIG_RAETH_HW_IOCOHERENT),y)
raeth-objs += ra_dbg_hwioc.o
endif

ccflags-y += -Werror
