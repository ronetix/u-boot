/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2019 NXP
 */

#ifndef __IMX8MM_COMPACT_CM_H
#define __IMX8MM_COMPACT_CM_H

#include <linux/sizes.h>
#include <asm/arch/imx-regs.h>
#include "imx_env.h"
#include "imx8m_ronetix_common.h"

#define CONFIG_SPL_MAX_SIZE				(148 * 1024)
#define CONFIG_SYS_MONITOR_LEN			SZ_512K
#define CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR	\
		(0x300 + CONFIG_SECONDARY_BOOT_SECTOR_OFFSET)

#ifdef CONFIG_SPL_BUILD
#define CONFIG_SPL_STACK			0x920000
#define CONFIG_SPL_BSS_START_ADDR	0x910000
#define CONFIG_SYS_SPL_MALLOC_SIZE	SZ_512K	/* 512 KB */

/* malloc f used before GD_FLG_FULL_MALLOC_INIT set */
#define CONFIG_MALLOC_F_ADDR		0x912000
#endif

#define CONFIG_SYS_INIT_RAM_SIZE        0x00200000
#define PHYS_SDRAM_SIZE					0x40000000 /* 1GB LPDDR */

#endif
