/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2019 NXP
 */

#ifndef __IMX8MM_COMPACT_CM_H
#define __IMX8MM_COMPACT_CM_H

#include <linux/sizes.h>
#include <linux/stringify.h>
#include <asm/arch/imx-regs.h>

#define UBOOT_ITB_OFFSET			0x57C00
#define FSPI_CONF_BLOCK_SIZE		0x1000
#define UBOOT_ITB_OFFSET_FSPI  \
	(UBOOT_ITB_OFFSET + FSPI_CONF_BLOCK_SIZE)
#ifdef CONFIG_FSPI_CONF_HEADER
#define CFG_SYS_UBOOT_BASE  \
	(QSPI0_AMBA_BASE + UBOOT_ITB_OFFSET_FSPI)
#else
#define CFG_SYS_UBOOT_BASE	\
	(QSPI0_AMBA_BASE + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512)
#endif

#ifdef CONFIG_XPL_BUILD
/* malloc f used before GD_FLG_FULL_MALLOC_INIT set */
#define CFG_MALLOC_F_ADDR		0x930000
/* For RAW image gives a error info not panic */

#endif

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
	"fdtfile=" CONFIG_DEFAULT_DEVICE_TREE ".dtb\0" \
	"bootm_size=0x10000000\0" \
	RONETIX_RAWBOOT_ENV \
	ENV_MEM_LAYOUT_SETTINGS

/* Link Definitions */

#define CFG_SYS_INIT_RAM_ADDR        0x40000000
#define CFG_SYS_INIT_RAM_SIZE        0x200000

#define CFG_SYS_SDRAM_BASE           0x40000000
#define PHYS_SDRAM                      0x40000000
#ifdef CONFIG_TARGET_IMX8MM_COMPACT_2GB_CM
#define PHYS_SDRAM_SIZE			0x80000000 /* 2GB LPDDR4 */
#else
#define PHYS_SDRAM_SIZE			0x40000000 /* 1GB LPDDR4 */
#endif

#ifdef CONFIG_TARGET_IMX8MM_COMPACT_2GB_CM
#define CFG_FEC_MXC_PHYADDR          1 /* KSZ9131RNX */
#else
#define CFG_FEC_MXC_PHYADDR          0 /* AR8031 */
#endif

#endif
