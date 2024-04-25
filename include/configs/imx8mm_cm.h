/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2019 NXP
 */

#ifndef __IMX8MM_CM_H
#define __IMX8MM_CM_H

#include <linux/sizes.h>
#include <linux/stringify.h>
#include <asm/arch/imx-regs.h>
#include "imx_env.h"

#ifdef CONFIG_SPL_BUILD
/* malloc f used before GD_FLG_FULL_MALLOC_INIT set */
#define CFG_MALLOC_F_ADDR		0x930000
/* For RAW image gives a error info not panic */

#endif

#define PHY_ANEG_TIMEOUT 20000
/*
 * Use:
 * 		boot-mode=mix
 * 		boot-mode=sd
 */
#define BOOT_MODE_ENV	\
	"boot-mode=sd\0" \
	"get_cmd=dhcp\0" \
	"serverip=192.168.3.5\0"

#define CFG_MFG_ENV_SETTINGS \
	"emmc_dev=2\0"\
	"sd_dev=1\0"

#define CONFIG_MMCROOT			"/dev/mmcblk1p2"  /* SD card, USDHC2 */
#define CONFIG_SYS_MMC_IMG_LOAD_PART	1

#define CFG_EXTRA_ENV_SETTINGS \
	BOOT_MODE_ENV \
	CFG_MFG_ENV_SETTINGS \
	"script=boot.scr\0" \
	"image=Image\0" \
	"splashimage=0x50000000\0" \
	"console=ttymxc1,115200\0" \
	"fdt_addr=0x43000000\0"			\
	"fdt_high=0xffffffffffffffff\0"		\
	"fdt_file=" CONFIG_DEFAULT_FDT_FILE "\0" \
	"initrd_addr=0x43800000\0"		\
	"initrd_high=0xffffffffffffffff\0" \
	"mmcdev="__stringify(CONFIG_SYS_MMC_ENV_DEV)"\0" \
	"mmcpart=" __stringify(CONFIG_SYS_MMC_IMG_LOAD_PART) "\0" \
	"mmcroot=" CONFIG_MMCROOT " rootwait rw\0" \
	"mmcargs=setenv bootargs console=${console} root=${mmcroot}\0 " \
	"loadimage=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${image}\0" \
	"loadfdt=fatload mmc ${mmcdev}:${mmcpart} ${fdt_addr} ${fdt_file}\0" \
	\
	"bootsd=echo Booting from SD card ...; " \
		"run mmcargs; " \
		"mmc dev ${mmcdev};" \
		"run loadfdt; " \
		"run loadimage; " \
		"booti ${loadaddr} - ${fdt_addr}; " \
		"\0" \
	\
	"bootmix=" \
		"echo Boot Kernel and FDT from TFTP, RootFs from SD card ...; " \
		"run mmcargs; " \
		"${get_cmd} ${fdt_addr} ${fdt_file}; " \
		"${get_cmd} ${image}; " \
		"booti ${loadaddr} - ${fdt_addr}; " \
		"\0"

/* Link Definitions */

#define CFG_SYS_INIT_RAM_ADDR        0x40000000
#define CFG_SYS_INIT_RAM_SIZE        0x200000

#define CFG_SYS_SDRAM_BASE           0x40000000
#define PHYS_SDRAM                   0x40000000
#define PHYS_SDRAM_SIZE				 0x40000000 /* 1GB DDR */

#define CFG_MXC_UART_BASE		UART_BASE_ADDR(2)
#define CFG_SYS_FSL_USDHC_NUM	2
#define CFG_SYS_FSL_ESDHC_ADDR	0
#endif
