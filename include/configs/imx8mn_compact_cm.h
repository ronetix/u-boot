/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2018 NXP
 */

#ifndef __IMX8MN_COMPACT_CM_H
#define __IMX8MN_COMPACT_CM_H

#include <linux/sizes.h>
#include <linux/stringify.h>
#include <asm/arch/imx-regs.h>

#define CFG_SYS_UBOOT_BASE	\
	(QSPI0_AMBA_BASE + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512)

#include "ronetix_imx8m.h"

/* per MMC device: distro boot first, then the Image/fdtfile fallback */
#define BOOT_TARGET_DEVICES(func) \
	func(MMC, mmc, 1) \
	func(RAWMMC, rawmmc, 1) \
	func(MMC, mmc, 2) \
	func(RAWMMC, rawmmc, 2) \
	func(DHCP, dhcp, na)

#include <config_distro_bootcmd.h>

/* Initial environment variables */
#define CFG_EXTRA_ENV_SETTINGS		\
	BOOTENV \
	"image=Image\0" \
	"console=ttymxc1,115200\0" \
	"boot_fit=no\0" \
	"fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0" \
	"bootm_size=0x10000000\0" \
	RONETIX_RAWBOOT_ENV \
	ENV_MEM_LAYOUT_SETTINGS

/* Link Definitions */

#define CFG_SYS_INIT_RAM_ADDR        0x40000000
#define CFG_SYS_INIT_RAM_SIZE        0x200000

#define CFG_SYS_SDRAM_BASE           0x40000000
#define PHYS_SDRAM                      0x40000000
#define PHYS_SDRAM_SIZE			0x40000000 /* 1GB LPDDR4 */

#define CFG_FEC_MXC_PHYADDR		0 /* AR8031 */

#endif
