/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2023 Renesas Electronics Corporation
 */

#ifndef __RNX_RZV2N_SMARC_H
#define __RNX_RZV2N_SMARC_H

#include <asm/arch/renesas.h>

#define CONFIG_REMAKE_ELF

#ifdef CONFIG_SPL
#define CONFIG_SPL_TARGET	"spl/u-boot-spl.scif"
#endif

/* boot option */

#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/* Generic Interrupt Controller Definitions */
/* RZ/V2N use GIC-v3 */
#define CONFIG_GICV3
#define GICD_BASE	0x14900000
#define GICR_BASE	0x14940000

/* console */
#define CONFIG_SYS_CBSIZE		2048
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_BAUDRATE_TABLE	{ 115200, 38400 }

/* PHY needs a longer autoneg timeout */
#define PHY_ANEG_TIMEOUT		20000

/* MEMORY */
#define CONFIG_SYS_INIT_SP_ADDR		CONFIG_SYS_TEXT_BASE

#define	DRAM_RSV_SIZE			0x08000000
#define	CFG_MAX_MEM_MAPPED		(0x80000000u - DRAM_RSV_SIZE)

/* SDHI clock freq */
#define CONFIG_SH_SDHI_FREQ		133000000


/* The HF/QSPI layout permits up to 1 MiB large bootloader blob */
#define CONFIG_BOARD_SIZE_LIMIT		1048576

/* ENV setting */
#define CFG_EXTRA_ENV_SETTINGS	\
	"serverip=192.168.3.5\0" \
	"ipaddr=192.168.3.111\0" \
	"usb_pgood_delay=2000\0" \
	"bootm_size=0x10000000\0" \
	"prodsdbootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk1p2 \0" \
	"prodemmcbootargs=setenv bootargs rw rootwait earlycon root=/dev/mmcblk0p2 \0" \
	"bootimage=booti 0x48080000 - 0x48000000 \0" \
	"emmcload=ext4load mmc 0:2 0x48080000 boot/Image;ext4load mmc 0:2 0x48000000 boot/r9a09g056n48-rzv2n-evk.dtb;run prodemmcbootargs \0" \
	"sd1load=ext4load mmc 1:3 0x48080000 boot/Image;ext4load mmc 1:3 0x48000000 boot/rzv2n-evk.dtb;run prodsdbootargs \0" \
	"netload=tftp 0x48080000 Image-rzv2n-evk.bin;tftp 0x48000000 Image-r9a09g056n44-evk.dtb;run prodsdbootargs \0" \
	"bootn=tftp 0x48080000 Image-rzv2n-evk.bin;tftp 0x48000000 Image-r9a09g056n44-evk.dtb;run prodsdbootargs;run bootimage \0" \
	"boots=run sd1load;run bootimage \0" \
	"bootcmd_check=mmc dev 1;run netload\0"

#define CONFIG_BOOTCOMMAND	"run bootcmd_check;run bootimage"

/* For board */
/* Ethernet RAVB */
#define CONFIG_BITBANGMII_MULTI

#endif /* __RNX_RZV2N_SMARC_H */
