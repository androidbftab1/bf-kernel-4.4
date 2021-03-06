CONFIG_ARM=y
CONFIG_ARM_HAS_SG_CHAIN=y
CONFIG_NEED_SG_DMA_LENGTH=y
CONFIG_ARM_DMA_USE_IOMMU=y
CONFIG_ARM_DMA_IOMMU_ALIGNMENT=8
CONFIG_MIGHT_HAVE_PCI=y
CONFIG_SYS_SUPPORTS_APM_EMULATION=y
CONFIG_HAVE_PROC_CPU=y
CONFIG_STACKTRACE_SUPPORT=y
CONFIG_LOCKDEP_SUPPORT=y
CONFIG_TRACE_IRQFLAGS_SUPPORT=y
CONFIG_RWSEM_XCHGADD_ALGORITHM=y
CONFIG_FIX_EARLYCON_MEM=y
CONFIG_GENERIC_HWEIGHT=y
CONFIG_GENERIC_CALIBRATE_DELAY=y
CONFIG_NEED_DMA_MAP_STATE=y
CONFIG_ARCH_SUPPORTS_UPROBES=y
CONFIG_VECTORS_BASE=0xffff0000
CONFIG_ARM_PATCH_PHYS_VIRT=y
CONFIG_GENERIC_BUG=y
CONFIG_PGTABLE_LEVELS=2
CONFIG_DEFCONFIG_LIST="/lib/modules/$UNAME_RELEASE/.config"
CONFIG_IRQ_WORK=y
CONFIG_BUILDTIME_EXTABLE_SORT=y

CONFIG_INIT_ENV_ARG_LIMIT=32
CONFIG_CROSS_COMPILE=""
CONFIG_LOCALVERSION=""
CONFIG_LOCALVERSION_AUTO=y
CONFIG_HAVE_KERNEL_GZIP=y
CONFIG_HAVE_KERNEL_LZMA=y
CONFIG_HAVE_KERNEL_XZ=y
CONFIG_HAVE_KERNEL_LZO=y
CONFIG_HAVE_KERNEL_LZ4=y
CONFIG_KERNEL_GZIP=y
CONFIG_DEFAULT_HOSTNAME="(none)"
CONFIG_SWAP=y
CONFIG_SYSVIPC=y
CONFIG_SYSVIPC_SYSCTL=y
CONFIG_CROSS_MEMORY_ATTACH=y
CONFIG_FHANDLE=y
CONFIG_USELIB=y
CONFIG_HAVE_ARCH_AUDITSYSCALL=y

CONFIG_GENERIC_IRQ_PROBE=y
CONFIG_GENERIC_IRQ_SHOW=y
CONFIG_GENERIC_IRQ_SHOW_LEVEL=y
CONFIG_HARDIRQS_SW_RESEND=y
CONFIG_IRQ_DOMAIN=y
CONFIG_IRQ_DOMAIN_HIERARCHY=y
CONFIG_HANDLE_DOMAIN_IRQ=y
CONFIG_IRQ_DOMAIN_DEBUG=y
CONFIG_IRQ_FORCED_THREADING=y
CONFIG_SPARSE_IRQ=y
CONFIG_GENERIC_TIME_VSYSCALL=y
CONFIG_GENERIC_CLOCKEVENTS=y
CONFIG_ARCH_HAS_TICK_BROADCAST=y
CONFIG_GENERIC_CLOCKEVENTS_BROADCAST=y

CONFIG_TICK_ONESHOT=y
CONFIG_NO_HZ_COMMON=y
CONFIG_NO_HZ_IDLE=y
CONFIG_NO_HZ=y
CONFIG_HIGH_RES_TIMERS=y

CONFIG_TICK_CPU_ACCOUNTING=y

CONFIG_TREE_RCU=y
CONFIG_SRCU=y
CONFIG_RCU_STALL_COMMON=y
CONFIG_LOG_BUF_SHIFT=17
CONFIG_LOG_CPU_MAX_BUF_SHIFT=12
CONFIG_GENERIC_SCHED_CLOCK=y
CONFIG_CGROUPS=y
CONFIG_NAMESPACES=y
CONFIG_UTS_NS=y
CONFIG_IPC_NS=y
CONFIG_PID_NS=y
CONFIG_NET_NS=y
CONFIG_BLK_DEV_INITRD=y
CONFIG_INITRAMFS_SOURCE="../rootfs"
CONFIG_INITRAMFS_ROOT_UID=0
CONFIG_INITRAMFS_ROOT_GID=0
CONFIG_RD_GZIP=y
CONFIG_RD_BZIP2=y
CONFIG_RD_LZMA=y
CONFIG_RD_XZ=y
CONFIG_RD_LZO=y
CONFIG_RD_LZ4=y
CONFIG_SYSCTL=y
CONFIG_ANON_INODES=y
CONFIG_HAVE_UID16=y
CONFIG_BPF=y
CONFIG_EXPERT=y
CONFIG_UID16=y
CONFIG_MULTIUSER=y
CONFIG_SYSFS_SYSCALL=y
CONFIG_KALLSYMS=y
CONFIG_PRINTK=y
CONFIG_BUG=y
CONFIG_ELF_CORE=y
CONFIG_BASE_FULL=y
CONFIG_FUTEX=y
CONFIG_EPOLL=y
CONFIG_SIGNALFD=y
CONFIG_TIMERFD=y
CONFIG_EVENTFD=y
CONFIG_SHMEM=y
CONFIG_AIO=y
CONFIG_ADVISE_SYSCALLS=y
CONFIG_PCI_QUIRKS=y
CONFIG_MEMBARRIER=y
CONFIG_EMBEDDED=y
CONFIG_HAVE_PERF_EVENTS=y
CONFIG_PERF_USE_VMALLOC=y

CONFIG_PERF_EVENTS=y
CONFIG_VM_EVENT_COUNTERS=y
CONFIG_SLUB_DEBUG=y
CONFIG_COMPAT_BRK=y
CONFIG_SLUB=y
CONFIG_SLUB_CPU_PARTIAL=y
CONFIG_TRACEPOINTS=y
CONFIG_KEXEC_CORE=y
CONFIG_HAVE_OPROFILE=y
CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS=y
CONFIG_ARCH_USE_BUILTIN_BSWAP=y
CONFIG_HAVE_KPROBES=y
CONFIG_HAVE_KRETPROBES=y
CONFIG_HAVE_OPTPROBES=y
CONFIG_HAVE_ARCH_TRACEHOOK=y
CONFIG_HAVE_DMA_ATTRS=y
CONFIG_HAVE_DMA_CONTIGUOUS=y
CONFIG_GENERIC_SMP_IDLE_THREAD=y
CONFIG_GENERIC_IDLE_POLL_SETUP=y
CONFIG_HAVE_REGS_AND_STACK_ACCESS_API=y
CONFIG_HAVE_CLK=y
CONFIG_HAVE_DMA_API_DEBUG=y
CONFIG_HAVE_HW_BREAKPOINT=y
CONFIG_HAVE_PERF_REGS=y
CONFIG_HAVE_PERF_USER_STACK_DUMP=y
CONFIG_HAVE_ARCH_JUMP_LABEL=y
CONFIG_ARCH_WANT_IPC_PARSE_VERSION=y
CONFIG_HAVE_ARCH_SECCOMP_FILTER=y
CONFIG_HAVE_CC_STACKPROTECTOR=y
CONFIG_CC_STACKPROTECTOR_NONE=y
CONFIG_HAVE_CONTEXT_TRACKING=y
CONFIG_HAVE_VIRT_CPU_ACCOUNTING_GEN=y
CONFIG_HAVE_IRQ_TIME_ACCOUNTING=y
CONFIG_HAVE_MOD_ARCH_SPECIFIC=y
CONFIG_MODULES_USE_ELF_REL=y
CONFIG_ARCH_HAS_ELF_RANDOMIZE=y
CONFIG_HAVE_ARCH_MMAP_RND_BITS=y
CONFIG_ARCH_MMAP_RND_BITS_MIN=8
CONFIG_ARCH_MMAP_RND_BITS_MAX=16
CONFIG_ARCH_MMAP_RND_BITS=8
CONFIG_CLONE_BACKWARDS=y
CONFIG_OLD_SIGSUSPEND3=y
CONFIG_OLD_SIGACTION=y

CONFIG_ARCH_HAS_GCOV_PROFILE_ALL=y
CONFIG_HAVE_GENERIC_DMA_COHERENT=y
CONFIG_SLABINFO=y
CONFIG_RT_MUTEXES=y
CONFIG_BASE_SMALL=0
CONFIG_MODULES=y
CONFIG_MODULE_FORCE_LOAD=y
CONFIG_MODULE_UNLOAD=y
CONFIG_MODULES_TREE_LOOKUP=y
CONFIG_BLOCK=y
CONFIG_LBDAF=y
CONFIG_BLK_DEV_BSG=y
CONFIG_BLK_CMDLINE_PARSER=y

CONFIG_PARTITION_ADVANCED=y
CONFIG_MSDOS_PARTITION=y
CONFIG_EFI_PARTITION=y
CONFIG_CMDLINE_PARTITION=y

CONFIG_IOSCHED_NOOP=y
CONFIG_IOSCHED_DEADLINE=y
CONFIG_IOSCHED_CFQ=y
CONFIG_DEFAULT_CFQ=y
CONFIG_DEFAULT_IOSCHED="cfq"
CONFIG_ASN1=y
CONFIG_INLINE_SPIN_UNLOCK_IRQ=y
CONFIG_INLINE_READ_UNLOCK=y
CONFIG_INLINE_READ_UNLOCK_IRQ=y
CONFIG_INLINE_WRITE_UNLOCK=y
CONFIG_INLINE_WRITE_UNLOCK_IRQ=y
CONFIG_ARCH_SUPPORTS_ATOMIC_RMW=y
CONFIG_MUTEX_SPIN_ON_OWNER=y
CONFIG_RWSEM_SPIN_ON_OWNER=y
CONFIG_LOCK_SPIN_ON_OWNER=y
CONFIG_FREEZER=y

CONFIG_MMU=y
CONFIG_ARCH_MULTIPLATFORM=y


CONFIG_ARCH_MULTI_V7=y
CONFIG_ARCH_MULTI_V6_V7=y
CONFIG_ARCH_MEDIATEK=y
CONFIG_MACH_MT7623=y


CONFIG_CPU_V7=y
CONFIG_CPU_32v6K=y
CONFIG_CPU_32v7=y
CONFIG_CPU_ABRT_EV7=y
CONFIG_CPU_PABRT_V7=y
CONFIG_CPU_CACHE_V7=y
CONFIG_CPU_CACHE_VIPT=y
CONFIG_CPU_COPY_V6=y
CONFIG_CPU_TLB_V7=y
CONFIG_CPU_HAS_ASID=y
CONFIG_CPU_CP15=y
CONFIG_CPU_CP15_MMU=y

CONFIG_ARM_THUMBEE=y
CONFIG_ARM_VIRT_EXT=y
CONFIG_SWP_EMULATE=y
CONFIG_KUSER_HELPERS=y
CONFIG_VDSO=y
CONFIG_OUTER_CACHE=y
CONFIG_OUTER_CACHE_SYNC=y
CONFIG_MIGHT_HAVE_CACHE_L2X0=y
CONFIG_CACHE_L2X0=y
CONFIG_PL310_ERRATA_588369=y
CONFIG_PL310_ERRATA_727915=y
CONFIG_PL310_ERRATA_753970=y
CONFIG_PL310_ERRATA_769419=y
CONFIG_ARM_L1_CACHE_SHIFT_6=y
CONFIG_ARM_L1_CACHE_SHIFT=6
CONFIG_ARM_DMA_MEM_BUFFERABLE=y
CONFIG_ARM_HEAVY_MB=y
CONFIG_MULTI_IRQ_HANDLER=y
CONFIG_ARM_ERRATA_643719=y
CONFIG_ARM_ERRATA_720789=y
CONFIG_ARM_ERRATA_754322=y
CONFIG_ARM_ERRATA_754327=y
CONFIG_ARM_ERRATA_764369=y
CONFIG_ARM_ERRATA_775420=y
CONFIG_ARM_ERRATA_798181=y

CONFIG_PCI=y
CONFIG_PCI_SYSCALL=y

CONFIG_PCI_MT2701=y

CONFIG_HAVE_SMP=y
CONFIG_SMP=y
CONFIG_SMP_ON_UP=y
CONFIG_ARM_CPU_TOPOLOGY=y
CONFIG_HAVE_ARM_ARCH_TIMER=y
CONFIG_VMSPLIT_3G=y
CONFIG_PAGE_OFFSET=0xC0000000
CONFIG_NR_CPUS=16
CONFIG_HOTPLUG_CPU=y
CONFIG_ARCH_NR_GPIO=0
CONFIG_PREEMPT_NONE=y
CONFIG_HZ_FIXED=0
CONFIG_HZ_100=y
CONFIG_HZ=100
CONFIG_SCHED_HRTICK=y
CONFIG_AEABI=y
CONFIG_HAVE_ARCH_PFN_VALID=y
CONFIG_HIGHMEM=y
CONFIG_HIGHPTE=y
CONFIG_CPU_SW_DOMAIN_PAN=y
CONFIG_HW_PERF_EVENTS=y
CONFIG_ARCH_WANT_GENERAL_HUGETLB=y
CONFIG_FLATMEM=y
CONFIG_FLAT_NODE_MEM_MAP=y
CONFIG_HAVE_MEMBLOCK=y
CONFIG_NO_BOOTMEM=y
CONFIG_MEMORY_ISOLATION=y
CONFIG_SPLIT_PTLOCK_CPUS=4
CONFIG_COMPACTION=y
CONFIG_MIGRATION=y
CONFIG_ZONE_DMA_FLAG=0
CONFIG_BOUNCE=y
CONFIG_DEFAULT_MMAP_MIN_ADDR=4096
CONFIG_CMA=y
CONFIG_CMA_AREAS=7
CONFIG_FORCE_MAX_ZONEORDER=12
CONFIG_ALIGNMENT_TRAP=y
CONFIG_SWIOTLB=y
CONFIG_IOMMU_HELPER=y

CONFIG_USE_OF=y
CONFIG_ATAGS=y
CONFIG_BUILD_ARM_APPENDED_DTB_IMAGE=y
CONFIG_BUILD_ARM_APPENDED_DTB_IMAGE_NAMES="mt7623n-evb"
CONFIG_ZBOOT_ROM_TEXT=0
CONFIG_ZBOOT_ROM_BSS=0
CONFIG_ARM_APPENDED_DTB=y
CONFIG_ARM_ATAG_DTB_COMPAT=y
CONFIG_ARM_ATAG_DTB_COMPAT_CMDLINE_FROM_BOOTLOADER=y
CONFIG_CMDLINE="earlyprintk console=ttyS0,115200 vmalloc=496M debug=7 initcall_debug=1"
CONFIG_CMDLINE_FORCE=y
CONFIG_KEXEC=y
CONFIG_ATAGS_PROC=y
CONFIG_AUTO_ZRELADDR=y





CONFIG_VFP=y
CONFIG_VFPv3=y
CONFIG_NEON=y
CONFIG_KERNEL_MODE_NEON=y

CONFIG_BINFMT_ELF=y
CONFIG_CORE_DUMP_DEFAULT_ELF_HEADERS=y
CONFIG_BINFMT_SCRIPT=y
CONFIG_BINFMT_MISC=y
CONFIG_COREDUMP=y

CONFIG_SUSPEND=y
CONFIG_SUSPEND_FREEZER=y
CONFIG_WAKELOCK=y
CONFIG_PM_SLEEP=y
CONFIG_PM_SLEEP_SMP=y
CONFIG_PM=y
CONFIG_PM_CLK=y
CONFIG_PM_GENERIC_DOMAINS=y
CONFIG_PM_GENERIC_DOMAINS_SLEEP=y
CONFIG_PM_GENERIC_DOMAINS_OF=y
CONFIG_CPU_PM=y
CONFIG_ARCH_SUSPEND_POSSIBLE=y
CONFIG_ARM_CPU_SUSPEND=y
CONFIG_ARCH_HIBERNATION_POSSIBLE=y
CONFIG_NET=y

CONFIG_PACKET=y
CONFIG_UNIX=y
CONFIG_XFRM=y
CONFIG_INET=y
CONFIG_IP_MULTICAST=y
CONFIG_NET_IPGRE_DEMUX=y
CONFIG_NET_IP_TUNNEL=y
CONFIG_INET_TUNNEL=y
CONFIG_INET_LRO=y
CONFIG_TCP_CONG_CUBIC=y
CONFIG_DEFAULT_TCP_CONG="cubic"
CONFIG_IPV6=y
CONFIG_IPV6_ROUTER_PREF=y
CONFIG_IPV6_ROUTE_INFO=y
CONFIG_INET6_XFRM_MODE_TRANSPORT=y
CONFIG_INET6_XFRM_MODE_TUNNEL=y
CONFIG_INET6_XFRM_MODE_BEET=y
CONFIG_IPV6_SIT=y
CONFIG_IPV6_NDISC_NODETYPE=y
CONFIG_IPV6_MROUTE=y
CONFIG_ANDROID_PARANOID_NETWORK=y
CONFIG_STP=y
CONFIG_BRIDGE=y
CONFIG_HAVE_NET_DSA=y
CONFIG_VLAN_8021Q=y
CONFIG_LLC=y
CONFIG_RPS=y
CONFIG_RFS_ACCEL=y
CONFIG_XPS=y
CONFIG_NET_RX_BUSY_POLL=y
CONFIG_BQL=y
CONFIG_NET_FLOW_LIMIT=y

CONFIG_WIRELESS=y

CONFIG_MAC80211_STA_HASH_MAX_SIZE=0
CONFIG_HAVE_BPF_JIT=y


CONFIG_UEVENT_HELPER=y
CONFIG_UEVENT_HELPER_PATH=""
CONFIG_DEVTMPFS=y
CONFIG_DEVTMPFS_MOUNT=y
CONFIG_STANDALONE=y
CONFIG_PREVENT_FIRMWARE_BUILD=y
CONFIG_FW_LOADER=y
CONFIG_FIRMWARE_IN_KERNEL=y
CONFIG_EXTRA_FIRMWARE=""
CONFIG_ALLOW_DEV_COREDUMP=y
CONFIG_REGMAP=y
CONFIG_REGMAP_MMIO=y
CONFIG_DMA_CMA=y

CONFIG_CMA_SIZE_MBYTES=64
CONFIG_CMA_SIZE_SEL_MBYTES=y
CONFIG_CMA_ALIGNMENT=8

CONFIG_ARM_CCI=y
CONFIG_ARM_CCI_PMU=y
CONFIG_ARM_CCI400_COMMON=y
CONFIG_ARM_CCI400_PMU=y
CONFIG_MTD=y
CONFIG_MTD_OF_PARTS=y


CONFIG_MTD_MAP_BANK_WIDTH_1=y
CONFIG_MTD_MAP_BANK_WIDTH_2=y
CONFIG_MTD_MAP_BANK_WIDTH_4=y
CONFIG_MTD_CFI_I1=y
CONFIG_MTD_CFI_I2=y




CONFIG_DTC=y
CONFIG_OF=y
CONFIG_OF_FLATTREE=y
CONFIG_OF_EARLY_FLATTREE=y
CONFIG_OF_DYNAMIC=y
CONFIG_OF_ADDRESS=y
CONFIG_OF_ADDRESS_PCI=y
CONFIG_OF_IRQ=y
CONFIG_OF_NET=y
CONFIG_OF_MDIO=y
CONFIG_OF_PCI=y
CONFIG_OF_PCI_IRQ=y
CONFIG_OF_MTD=y
CONFIG_OF_RESERVED_MEM=y
CONFIG_OF_RESOLVE=y
CONFIG_OF_OVERLAY=y
CONFIG_ARCH_MIGHT_HAVE_PC_PARPORT=y
CONFIG_BLK_DEV=y
CONFIG_BLK_DEV_LOOP=y
CONFIG_BLK_DEV_LOOP_MIN_COUNT=8

CONFIG_SRAM=y

CONFIG_EEPROM_93CX6=y









CONFIG_MTK_ICE_DEBUG=y



CONFIG_HAVE_IDE=y

CONFIG_SCSI_MOD=y
CONFIG_SCSI=y
CONFIG_SCSI_DMA=y
CONFIG_SCSI_PROC_FS=y

CONFIG_BLK_DEV_SD=y

CONFIG_SCSI_LOWLEVEL=y

CONFIG_NETDEVICES=y
CONFIG_NET_CORE=y


CONFIG_ETHERNET=y
CONFIG_NET_VENDOR_3COM=y
CONFIG_NET_VENDOR_ADAPTEC=y
CONFIG_NET_VENDOR_AGERE=y
CONFIG_NET_VENDOR_ALTEON=y
CONFIG_NET_VENDOR_AMD=y
CONFIG_NET_VENDOR_ARC=y
CONFIG_NET_VENDOR_ATHEROS=y
CONFIG_NET_CADENCE=y
CONFIG_NET_VENDOR_BROADCOM=y
CONFIG_NET_VENDOR_BROCADE=y
CONFIG_NET_VENDOR_CAVIUM=y
CONFIG_NET_VENDOR_CHELSIO=y
CONFIG_NET_VENDOR_CIRRUS=y
CONFIG_NET_VENDOR_CISCO=y
CONFIG_NET_VENDOR_DEC=y
CONFIG_NET_VENDOR_DLINK=y
CONFIG_NET_VENDOR_EMULEX=y
CONFIG_NET_VENDOR_EZCHIP=y
CONFIG_NET_VENDOR_EXAR=y
CONFIG_NET_VENDOR_FARADAY=y
CONFIG_NET_VENDOR_HISILICON=y
CONFIG_NET_VENDOR_HP=y
CONFIG_NET_VENDOR_INTEL=y
CONFIG_NET_VENDOR_I825XX=y
CONFIG_NET_VENDOR_MARVELL=y
CONFIG_NET_VENDOR_MEDIATEK=y
CONFIG_NET_MEDIATEK_SOC=y
CONFIG_NET_VENDOR_MELLANOX=y
CONFIG_NET_VENDOR_MICREL=y
CONFIG_NET_VENDOR_MYRI=y
CONFIG_NET_VENDOR_NATSEMI=y
CONFIG_NET_VENDOR_8390=y
CONFIG_NET_VENDOR_NVIDIA=y
CONFIG_NET_VENDOR_OKI=y
CONFIG_NET_PACKET_ENGINE=y
CONFIG_NET_VENDOR_QLOGIC=y
CONFIG_NET_VENDOR_QUALCOMM=y
CONFIG_NET_VENDOR_REALTEK=y
CONFIG_NET_VENDOR_RENESAS=y
CONFIG_NET_VENDOR_RDC=y
CONFIG_NET_VENDOR_ROCKER=y
CONFIG_NET_VENDOR_SAMSUNG=y
CONFIG_NET_VENDOR_SEEQ=y
CONFIG_NET_VENDOR_SILAN=y
CONFIG_NET_VENDOR_SIS=y
CONFIG_NET_VENDOR_SMSC=y
CONFIG_NET_VENDOR_STMICRO=y
CONFIG_NET_VENDOR_SUN=y
CONFIG_NET_VENDOR_SYNOPSYS=y
CONFIG_NET_VENDOR_TEHUTI=y
CONFIG_NET_VENDOR_TI=y
CONFIG_NET_VENDOR_VIA=y
CONFIG_NET_VENDOR_WIZNET=y
CONFIG_PHYLIB=y

CONFIG_ICPLUS_PHY=y
CONFIG_FIXED_PHY=y
CONFIG_USB_NET_DRIVERS=y
CONFIG_WLAN=y


CONFIG_INPUT=y
CONFIG_INPUT_MATRIXKMAP=y

CONFIG_INPUT_MOUSEDEV=y
CONFIG_INPUT_MOUSEDEV_PSAUX=y
CONFIG_INPUT_MOUSEDEV_SCREEN_X=1024
CONFIG_INPUT_MOUSEDEV_SCREEN_Y=768
CONFIG_INPUT_EVDEV=y

CONFIG_INPUT_KEYBOARD=y
CONFIG_KPD_PMIC_LPRST_TD=0
CONFIG_KEYBOARD_ATKBD=y
CONFIG_KEYBOARD_MATRIX=y
CONFIG_KEYBOARD_SAMSUNG=y
CONFIG_INPUT_MOUSE=y
CONFIG_MOUSE_PS2=y
CONFIG_MOUSE_PS2_ALPS=y
CONFIG_MOUSE_PS2_LOGIPS2PP=y
CONFIG_MOUSE_PS2_SYNAPTICS=y
CONFIG_MOUSE_PS2_CYPRESS=y
CONFIG_MOUSE_PS2_TRACKPOINT=y
CONFIG_MOUSE_PS2_ELANTECH=y
CONFIG_MOUSE_PS2_SENTELIC=y
CONFIG_MOUSE_PS2_FOCALTECH=y
CONFIG_INPUT_TOUCHSCREEN=y
CONFIG_TOUCHSCREEN_PROPERTIES=y

CONFIG_SERIO=y
CONFIG_SERIO_LIBPS2=y

CONFIG_TTY=y
CONFIG_VT=y
CONFIG_CONSOLE_TRANSLATIONS=y
CONFIG_VT_CONSOLE=y
CONFIG_VT_CONSOLE_SLEEP=y
CONFIG_HW_CONSOLE=y
CONFIG_VT_HW_CONSOLE_BINDING=y
CONFIG_UNIX98_PTYS=y
CONFIG_LEGACY_PTYS=y
CONFIG_LEGACY_PTY_COUNT=256
CONFIG_DEVMEM=y
CONFIG_DEVKMEM=y

CONFIG_SERIAL_EARLYCON=y
CONFIG_SERIAL_8250=y
CONFIG_SERIAL_8250_DEPRECATED_OPTIONS=y
CONFIG_SERIAL_8250_CONSOLE=y
CONFIG_SERIAL_8250_DMA=y
CONFIG_SERIAL_8250_PCI=y
CONFIG_SERIAL_8250_NR_UARTS=4
CONFIG_SERIAL_8250_RUNTIME_UARTS=4
CONFIG_SERIAL_8250_FSL=y
CONFIG_SERIAL_8250_MT6577=y

CONFIG_SERIAL_CORE=y
CONFIG_SERIAL_CORE_CONSOLE=y
CONFIG_HW_RANDOM=m
CONFIG_DEVPORT=y

CONFIG_I2C=y
CONFIG_I2C_BOARDINFO=y
CONFIG_I2C_COMPAT=y
CONFIG_I2C_HELPER_AUTO=y



CONFIG_I2C_MT65XX=y






CONFIG_PINCTRL=y

CONFIG_PINMUX=y
CONFIG_PINCONF=y
CONFIG_GENERIC_PINCONF=y
CONFIG_PINCTRL_MTK_COMMON=y
CONFIG_PINCTRL_MT2701=y
CONFIG_PINCTRL_MT6397=y
CONFIG_ARCH_HAVE_CUSTOM_GPIO_H=y
CONFIG_ARCH_WANT_OPTIONAL_GPIOLIB=y
CONFIG_GPIOLIB=y
CONFIG_GPIO_DEVRES=y
CONFIG_OF_GPIO=y






CONFIG_WATCHDOG=y
CONFIG_WATCHDOG_CORE=y

CONFIG_MEDIATEK_WATCHDOG=y


CONFIG_SSB_POSSIBLE=y

CONFIG_BCMA_POSSIBLE=y


CONFIG_MFD_CORE=y
CONFIG_MFD_MT6397=y
CONFIG_MFD_SYSCON=y
CONFIG_REGULATOR=y
CONFIG_REGULATOR_MT6323=y


CONFIG_VGA_ARB=y
CONFIG_VGA_ARB_MAX_GPUS=16


CONFIG_DUMMY_CONSOLE=y

CONFIG_HID=y
CONFIG_HID_GENERIC=y


CONFIG_USB_HID=y

CONFIG_USB_OHCI_LITTLE_ENDIAN=y
CONFIG_USB_SUPPORT=y
CONFIG_USB_COMMON=y
CONFIG_USB_ARCH_HAS_HCD=y
CONFIG_USB=y

CONFIG_USB_DEFAULT_PERSIST=y

CONFIG_USB_XHCI_HCD=y
CONFIG_USB_XHCI_PCI=y



CONFIG_USB_STORAGE=y




CONFIG_USB_PHY=y
CONFIG_NOP_USB_XCEIV=y
CONFIG_USB_GPIO_VBUS=y
CONFIG_USB_GADGET=y
CONFIG_USB_GADGET_VBUS_DRAW=2
CONFIG_USB_GADGET_STORAGE_NUM_BUFFERS=2

CONFIG_USB_LIBCOMPOSITE=y
CONFIG_USB_F_ACM=y
CONFIG_USB_U_SERIAL=y
CONFIG_USB_U_ETHER=y
CONFIG_USB_F_SERIAL=y
CONFIG_USB_F_OBEX=y
CONFIG_USB_F_NCM=y
CONFIG_USB_F_ECM=y
CONFIG_USB_F_EEM=y
CONFIG_USB_F_SUBSET=y
CONFIG_USB_F_RNDIS=y
CONFIG_USB_F_MASS_STORAGE=y
CONFIG_USB_CONFIGFS=y
CONFIG_USB_CONFIGFS_SERIAL=y
CONFIG_USB_CONFIGFS_ACM=y
CONFIG_USB_CONFIGFS_OBEX=y
CONFIG_USB_CONFIGFS_NCM=y
CONFIG_USB_CONFIGFS_ECM=y
CONFIG_USB_CONFIGFS_ECM_SUBSET=y
CONFIG_USB_CONFIGFS_RNDIS=y
CONFIG_USB_CONFIGFS_EEM=y
CONFIG_USB_CONFIGFS_MASS_STORAGE=y
CONFIG_MMC=y

CONFIG_MMC_BLOCK=y
CONFIG_MMC_BLOCK_MINORS=8
CONFIG_MMC_BLOCK_BOUNCE=y

CONFIG_EDAC_ATOMIC_SCRUB=y
CONFIG_EDAC_SUPPORT=y
CONFIG_RTC_LIB=y
CONFIG_DMADEVICES=y

CONFIG_DMA_ENGINE=y
CONFIG_DMA_OF=y



CONFIG_CLKDEV_LOOKUP=y
CONFIG_HAVE_CLK_PREPARE=y
CONFIG_COMMON_CLK=y

CONFIG_COMMON_CLK_MEDIATEK=y
CONFIG_COMMON_CLK_MT2701=y
CONFIG_COMMON_CLK_MT8135=y
CONFIG_COMMON_CLK_MT8173=y


CONFIG_CLKSRC_OF=y
CONFIG_CLKSRC_PROBE=y
CONFIG_CLKSRC_MMIO=y
CONFIG_ARM_ARCH_TIMER=y
CONFIG_ARM_ARCH_TIMER_EVTSTREAM=y
CONFIG_ARM_TIMER_SP804=y
CONFIG_MTK_TIMER=y
CONFIG_IOMMU_API=y
CONFIG_IOMMU_SUPPORT=y

CONFIG_OF_IOMMU=y
CONFIG_MTK_IOMMU_V1=y



CONFIG_MTK_INFRACFG=y
CONFIG_MTK_PMIC_WRAP=y
CONFIG_MTK_SCPSYS=y
CONFIG_MEMORY=y
CONFIG_MTK_SMI=y
CONFIG_IIO=y





















CONFIG_IRQCHIP=y
CONFIG_ARM_GIC=y
CONFIG_RESET_CONTROLLER=y

CONFIG_GENERIC_PHY=y
CONFIG_PHY_MT65XX_USB3=y

CONFIG_ARM_PMU=y
CONFIG_RAS=y




CONFIG_DCACHE_WORD_ACCESS=y
CONFIG_EXT4_FS=y
CONFIG_EXT4_USE_FOR_EXT2=y
CONFIG_JBD2=y
CONFIG_FS_MBCACHE=y
CONFIG_FS_POSIX_ACL=y
CONFIG_EXPORTFS=y
CONFIG_FILE_LOCKING=y
CONFIG_FSNOTIFY=y
CONFIG_DNOTIFY=y
CONFIG_INOTIFY_USER=y
CONFIG_AUTOFS4_FS=y
CONFIG_FUSE_FS=m



CONFIG_FAT_FS=m
CONFIG_MSDOS_FS=m
CONFIG_VFAT_FS=m
CONFIG_FAT_DEFAULT_CODEPAGE=437
CONFIG_FAT_DEFAULT_IOCHARSET="iso8859-1"
CONFIG_NTFS_FS=m

CONFIG_PROC_FS=y
CONFIG_PROC_SYSCTL=y
CONFIG_PROC_PAGE_MONITOR=y
CONFIG_KERNFS=y
CONFIG_SYSFS=y
CONFIG_TMPFS=y
CONFIG_TMPFS_POSIX_ACL=y
CONFIG_TMPFS_XATTR=y
CONFIG_CONFIGFS_FS=y
CONFIG_MISC_FILESYSTEMS=y
CONFIG_PSTORE=y
CONFIG_PSTORE_CONSOLE=y
CONFIG_PSTORE_PMSG=y
CONFIG_PSTORE_FTRACE=y
CONFIG_PSTORE_RAM=y
CONFIG_NETWORK_FILESYSTEMS=y
CONFIG_NLS=y
CONFIG_NLS_DEFAULT="iso8859-1"
CONFIG_NLS_CODEPAGE_437=m
CONFIG_NLS_ISO8859_1=m
CONFIG_NLS_UTF8=y


CONFIG_PRINTK_TIME=y
CONFIG_MESSAGE_LOGLEVEL_DEFAULT=4

CONFIG_DEBUG_INFO=y
CONFIG_ENABLE_WARN_DEPRECATED=y
CONFIG_ENABLE_MUST_CHECK=y
CONFIG_FRAME_WARN=1024
CONFIG_DEBUG_FS=y
CONFIG_SECTION_MISMATCH_WARN_ONLY=y
CONFIG_FRAME_POINTER=y
CONFIG_MAGIC_SYSRQ=y
CONFIG_MAGIC_SYSRQ_DEFAULT_ENABLE=0x1
CONFIG_DEBUG_KERNEL=y

CONFIG_HAVE_DEBUG_KMEMLEAK=y

CONFIG_LOCKUP_DETECTOR=y
CONFIG_HARDLOCKUP_DETECTOR_OTHER_CPU=y
CONFIG_HARDLOCKUP_DETECTOR=y
CONFIG_BOOTPARAM_HARDLOCKUP_PANIC_VALUE=0
CONFIG_BOOTPARAM_SOFTLOCKUP_PANIC_VALUE=0
CONFIG_DETECT_HUNG_TASK=y
CONFIG_DEFAULT_HUNG_TASK_TIMEOUT=120
CONFIG_BOOTPARAM_HUNG_TASK_PANIC_VALUE=0
CONFIG_PANIC_ON_OOPS_VALUE=0
CONFIG_PANIC_TIMEOUT=0
CONFIG_SCHED_DEBUG=y

CONFIG_STACKTRACE=y
CONFIG_DEBUG_BUGVERBOSE=y

CONFIG_RCU_CPU_STALL_TIMEOUT=21
CONFIG_NOP_TRACER=y
CONFIG_HAVE_FUNCTION_TRACER=y
CONFIG_HAVE_FUNCTION_GRAPH_TRACER=y
CONFIG_HAVE_DYNAMIC_FTRACE=y
CONFIG_HAVE_FTRACE_MCOUNT_RECORD=y
CONFIG_HAVE_SYSCALL_TRACEPOINTS=y
CONFIG_HAVE_C_RECORDMCOUNT=y
CONFIG_TRACE_CLOCK=y
CONFIG_RING_BUFFER=y
CONFIG_EVENT_TRACING=y
CONFIG_CONTEXT_SWITCH_TRACER=y
CONFIG_TRACING=y
CONFIG_GENERIC_TRACER=y
CONFIG_TRACING_SUPPORT=y
CONFIG_FTRACE=y
CONFIG_FUNCTION_TRACER=y
CONFIG_FUNCTION_GRAPH_TRACER=y
CONFIG_FTRACE_SYSCALLS=y
CONFIG_BRANCH_PROFILE_NONE=y
CONFIG_DYNAMIC_FTRACE=y
CONFIG_FUNCTION_PROFILER=y
CONFIG_FTRACE_MCOUNT_RECORD=y
CONFIG_TRACING_EVENTS_GPIO=y

CONFIG_HAVE_ARCH_KGDB=y
CONFIG_ARM_UNWIND=y
CONFIG_OLD_MCOUNT=y
CONFIG_DEBUG_LL_INCLUDE="mach/debug-macro.S"
CONFIG_UNCOMPRESS_INCLUDE="debug/uncompress.h"

CONFIG_KEYS=y
CONFIG_DEFAULT_SECURITY_DAC=y
CONFIG_DEFAULT_SECURITY=""
CONFIG_CRYPTO=y

CONFIG_CRYPTO_ALGAPI=y
CONFIG_CRYPTO_ALGAPI2=y
CONFIG_CRYPTO_AEAD=m
CONFIG_CRYPTO_AEAD2=y
CONFIG_CRYPTO_BLKCIPHER=m
CONFIG_CRYPTO_BLKCIPHER2=y
CONFIG_CRYPTO_HASH=y
CONFIG_CRYPTO_HASH2=y
CONFIG_CRYPTO_RNG=m
CONFIG_CRYPTO_RNG2=y
CONFIG_CRYPTO_RNG_DEFAULT=m
CONFIG_CRYPTO_PCOMP2=y
CONFIG_CRYPTO_AKCIPHER2=y
CONFIG_CRYPTO_AKCIPHER=y
CONFIG_CRYPTO_RSA=y
CONFIG_CRYPTO_MANAGER=m
CONFIG_CRYPTO_MANAGER2=y
CONFIG_CRYPTO_MANAGER_DISABLE_TESTS=y
CONFIG_CRYPTO_GF128MUL=m
CONFIG_CRYPTO_NULL=m
CONFIG_CRYPTO_NULL2=y
CONFIG_CRYPTO_WORKQUEUE=y
CONFIG_CRYPTO_CRYPTD=m
CONFIG_CRYPTO_AUTHENC=m
CONFIG_CRYPTO_ABLK_HELPER=m

CONFIG_CRYPTO_CCM=m
CONFIG_CRYPTO_GCM=m
CONFIG_CRYPTO_SEQIV=m
CONFIG_CRYPTO_ECHAINIV=m

CONFIG_CRYPTO_CBC=m
CONFIG_CRYPTO_CTR=m
CONFIG_CRYPTO_ECB=m

CONFIG_CRYPTO_CMAC=m
CONFIG_CRYPTO_HMAC=m

CONFIG_CRYPTO_CRC32C=y
CONFIG_CRYPTO_GHASH=m
CONFIG_CRYPTO_MD5=m
CONFIG_CRYPTO_SHA1=m
CONFIG_CRYPTO_SHA256=m

CONFIG_CRYPTO_AES=y
CONFIG_CRYPTO_ARC4=m
CONFIG_CRYPTO_DES=m

CONFIG_CRYPTO_DEFLATE=y
CONFIG_CRYPTO_LZO=y

CONFIG_CRYPTO_DRBG_MENU=m
CONFIG_CRYPTO_DRBG_HMAC=y
CONFIG_CRYPTO_DRBG=m
CONFIG_CRYPTO_JITTERENTROPY=m
CONFIG_CRYPTO_HW=y

CONFIG_ARM_CRYPTO=y
CONFIG_CRYPTO_SHA1_ARM=m
CONFIG_CRYPTO_SHA1_ARM_NEON=m
CONFIG_CRYPTO_SHA1_ARM_CE=m
CONFIG_CRYPTO_SHA2_ARM_CE=m
CONFIG_CRYPTO_SHA256_ARM=m
CONFIG_CRYPTO_SHA512_ARM=m
CONFIG_CRYPTO_AES_ARM=m
CONFIG_CRYPTO_AES_ARM_BS=m
CONFIG_CRYPTO_AES_ARM_CE=m
CONFIG_CRYPTO_GHASH_ARM_CE=m
CONFIG_BINARY_PRINTF=y

CONFIG_BITREVERSE=y
CONFIG_HAVE_ARCH_BITREVERSE=y
CONFIG_RATIONAL=y
CONFIG_GENERIC_STRNCPY_FROM_USER=y
CONFIG_GENERIC_STRNLEN_USER=y
CONFIG_GENERIC_NET_UTILS=y
CONFIG_GENERIC_PCI_IOMAP=y
CONFIG_GENERIC_IO=y
CONFIG_ARCH_USE_CMPXCHG_LOCKREF=y
CONFIG_CRC_CCITT=m
CONFIG_CRC16=y
CONFIG_CRC_ITU_T=m
CONFIG_CRC32=y
CONFIG_CRC32_SLICEBY8=y
CONFIG_ZLIB_INFLATE=y
CONFIG_ZLIB_DEFLATE=y
CONFIG_LZO_COMPRESS=y
CONFIG_LZO_DECOMPRESS=y
CONFIG_LZ4_DECOMPRESS=y
CONFIG_XZ_DEC=y
CONFIG_XZ_DEC_X86=y
CONFIG_XZ_DEC_POWERPC=y
CONFIG_XZ_DEC_IA64=y
CONFIG_XZ_DEC_ARM=y
CONFIG_XZ_DEC_ARMTHUMB=y
CONFIG_XZ_DEC_SPARC=y
CONFIG_XZ_DEC_BCJ=y
CONFIG_DECOMPRESS_GZIP=y
CONFIG_DECOMPRESS_BZIP2=y
CONFIG_DECOMPRESS_LZMA=y
CONFIG_DECOMPRESS_XZ=y
CONFIG_DECOMPRESS_LZO=y
CONFIG_DECOMPRESS_LZ4=y
CONFIG_GENERIC_ALLOCATOR=y
CONFIG_REED_SOLOMON=y
CONFIG_REED_SOLOMON_ENC8=y
CONFIG_REED_SOLOMON_DEC8=y
CONFIG_ASSOCIATIVE_ARRAY=y
CONFIG_HAS_IOMEM=y
CONFIG_HAS_IOPORT_MAP=y
CONFIG_HAS_DMA=y
CONFIG_CPU_RMAP=y
CONFIG_DQL=y
CONFIG_NLATTR=y
CONFIG_ARCH_HAS_ATOMIC64_DEC_IF_POSITIVE=y
CONFIG_CLZ_TAB=y
CONFIG_MPILIB=y
CONFIG_LIBFDT=y
CONFIG_ARCH_HAS_SG_CHAIN=y


CONFIG_LIB_LIBGMP_FORCE=y
CONFIG_LIB_LIBPTHREAD_FORCE=y
CONFIG_LIB_LIBNVRAM_FORCE=y
CONFIG_LIB_FLEX_FORCE=y
CONFIG_LIB_PCRE_FORCE=y
CONFIG_LIB_ZLIB_FORCE=y


CONFIG_USER_ACCEL_PPTP=y
CONFIG_USER_PPPD_WITH_PPTP=y
CONFIG_USER_ISC_DHCP=y
CONFIG_USER_STORAGE=y
CONFIG_USER_LIGHTY=y
CONFIG_LIB_PCRE_FORCE=y
CONFIG_LIB_ZLIB_FORCE=y
CONFIG_USER_ECMH=y
CONFIG_USER_IPTABLES_IPTABLES=y
CONFIG_LIB_LIBM_FORCE=y
CONFIG_USER_IPROUTE2=y
CONFIG_USER_IPROUTE2_IP=y
CONFIG_LIB_LIBM_FORCE=y
CONFIG_USER_OPENL2TP=y
CONFIG_USER_PPPD_WITH_L2TP=y
CONFIG_LIB_FLEX_FORCE=y
CONFIG_USER_OPENSWAN=y
CONFIG_USER_OPENSWAN_PLUTO_PLUTO=y
CONFIG_USER_OPENSWAN_PLUTO_WHACK=y
CONFIG_USER_OPENSWAN_UTILS_RANBITS=y
CONFIG_USER_OPENSWAN_UTILS_RSASIGKEY=y
CONFIG_USER_OPENSWAN_KLIPS_EROUTE=y
CONFIG_USER_OPENSWAN_KLIPS_KLIPSDEBUG=y
CONFIG_USER_OPENSWAN_KLIPS_SPI=y
CONFIG_USER_OPENSWAN_KLIPS_SPIGRP=y
CONFIG_USER_OPENSWAN_KLIPS_TNCFG=y
CONFIG_LIB_LIBGMP_FORCE=y
CONFIG_LIB_CRYPT_FORCE=y
CONFIG_LIB_RESOLV_FORCE=y
CONFIG_USER_PPPD=y
CONFIG_USER_PPPD_WITH_PPPOE=y
CONFIG_USER_PPPD_WITH_L2TP=y
CONFIG_USER_PPPD_WITH_PPTP=y
CONFIG_USER_RADVD=y
CONFIG_USER_802_1X=y

CONFIG_USER_WIRELESS_TOOLS=y
CONFIG_USER_WIRELESS_TOOLS_IWCONFIG=y
CONFIG_USER_WIRELESS_TOOLS_IWLIST=y
CONFIG_USER_WIRELESS_TOOLS_IWPRIV=y
CONFIG_USER_RALINKIAPPD=y

CONFIG_USER_BUSYBOX_BUSYBOX=y
CONFIG_USER_IXIA_ENDPOINT=y
CONFIG_USER_IXIA_ENDPOINT_730SP1=y
CONFIG_USER_MTD_WRITE=y
CONFIG_USER_MPSTAT=y
CONFIG_USER_TCPDUMP=y
CONFIG_USER_PCIUTIL_LSPCI=y
CONFIG_LIB_ZLIB_FORCE=y

CONFIG_RALINKAPP=y
CONFIG_RALINKAPP_ATED=y
CONFIG_RALINKAPP_FLASH=y
CONFIG_FLASH_TOOL=y
CONFIG_ETHMAC_TOOL=y
CONFIG_RALINKAPP_NVRAM=y
CONFIG_LIB_LIBNVRAM_FORCE=y
CONFIG_NVRAM_RW_FILE=y
CONFIG_RALINKAPP_SCRIPTS=y



