/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2018 NXP
 */

#ifndef __IMX8MN_CM_H
#define __IMX8MN_CM_H

#include <linux/sizes.h>
#include <asm/arch/imx-regs.h>
#include "imx_env.h"
#include "imx8m_ronetix_common.h"

#define CONFIG_SPL_MAX_SIZE							(208 * 1024)
#define CONFIG_SYS_MONITOR_LEN						(512 * 1024)
#define CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR		0x300

#ifdef CONFIG_SPL_BUILD
#define CONFIG_SPL_STACK				0x187FF0
#define CONFIG_SPL_BSS_START_ADDR   	0x0095e000
#define CONFIG_SYS_SPL_MALLOC_SIZE		SZ_64K	/* 64 KB */

/* malloc f used before GD_FLG_FULL_MALLOC_INIT set */
#define CONFIG_MALLOC_F_ADDR			0x184000
#endif

#define CONFIG_SYS_INIT_RAM_SIZE        0x00080000
#define PHYS_SDRAM_SIZE					0x20000000 /* 512 MB LPDDR4 */

#endif
