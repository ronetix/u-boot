/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2026 Ronetix GmbH
 *
 * Environment shared by the Ronetix i.MX8M boards.
 *
 * Boot order per MMC device (see BOOT_TARGET_DEVICES in the board header):
 *   mmcX    - distro boot: extlinux/extlinux.conf, boot.scr, EFI
 *   rawmmcX - fallback: ${image} and ${fdtfile} from the FAT partition
 *             ${mmcpart}, root file system on partition ${rootpart}
 *
 * The fallback boots a card which contains only the kernel Image and the
 * device tree (no extlinux.conf), e.g. prepared by hand outside of Yocto.
 * Extra kernel arguments can be set in ${optargs}.
 */

#ifndef __RONETIX_IMX8M_H
#define __RONETIX_IMX8M_H

/* distro boot target "rawmmcX" */
#define BOOTENV_DEV_RAWMMC(devtypeu, devtypel, instance) \
	"bootcmd_" #devtypel #instance "=" \
		"setenv mmcdev " #instance "; run mmc_rawboot\0"

#define BOOTENV_DEV_NAME_RAWMMC(devtypeu, devtypel, instance) \
	#devtypel #instance " "

/*
 * U-Boot and Linux use the same MMC numbering (aliases mmc0..mmc2),
 * so the root device is /dev/mmcblk${mmcdev}p${rootpart}.
 */
#define RONETIX_RAWBOOT_ENV \
	"mmcpart=1\0" \
	"rootpart=2\0" \
	"mmc_rawboot=" \
		"if mmc dev ${mmcdev} && " \
		"fatload mmc ${mmcdev}:${mmcpart} ${kernel_addr_r} ${image} && " \
		"fatload mmc ${mmcdev}:${mmcpart} ${fdt_addr_r} ${fdtfile}; then " \
			"echo Booting ${image} from mmc ${mmcdev}:${mmcpart} ...; " \
			"setenv bootargs console=${console} " \
				"root=/dev/mmcblk${mmcdev}p${rootpart} rootwait rw " \
				"${optargs}; " \
			"booti ${kernel_addr_r} - ${fdt_addr_r}; " \
		"fi\0"

/*
 * Memory layout for distro boot: room for a kernel Image of up to 96 MiB
 * at kernel_addr_r before fdt_addr_r.
 */
#define ENV_MEM_LAYOUT_SETTINGS \
	"loadaddr=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
	"kernel_addr_r=0x42000000\0" \
	"fdt_addr_r=0x48000000\0" \
	"fdtoverlay_addr_r=0x49000000\0" \
	"ramdisk_addr_r=0x48080000\0" \
	"initrd_addr=0x48080000\0" \
	"scriptaddr=0x40000000\0" \
	"pxefile_addr_r=0x40100000\0"

#endif
