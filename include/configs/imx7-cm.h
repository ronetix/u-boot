/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2015 Freescale Semiconductor, Inc.
 *
 * Configuration settings for the Ronetix i.MX7-CM board.
 */

#ifndef __IMX7CM_CONFIG_H
#define __IMX7CM_CONFIG_H

#include "mx7_common.h"

#define PHYS_SDRAM_SIZE				SZ_512M

#define CONFIG_MXC_UART_BASE		UART1_IPS_BASE_ADDR

/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(32 * SZ_1M)

/* Network */
#define CONFIG_FEC_MXC
#define CONFIG_FEC_XCV_TYPE         RGMII
#define CONFIG_ETHPRIME             "FEC"

/* ENET1 */
#define IMX_FEC_BASE				ENET_IPS_BASE_ADDR

/* MMC Config*/
#define CONFIG_SYS_FSL_ESDHC_ADDR   0

/* I2C configs */
/* I2C EEPROM */
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	2
#define CONFIG_SYS_I2C_MXC
#define CONFIG_SYS_I2C_SPEED			100000

#define CONFIG_SYS_MMC_IMG_LOAD_PART	1

#ifdef CONFIG_FSL_QSPI
#define CONFIG_SYS_FSL_QSPI_AHB
#define CONFIG_SF_DEFAULT_CS		0
#define FSL_QSPI_FLASH_NUM			1
#define FSL_QSPI_FLASH_SIZE			SZ_32M
#define QSPI0_BASE_ADDR				QSPI1_IPS_BASE_ADDR
#define QSPI0_AMBA_BASE				QSPI0_ARB_BASE_ADDR
#endif

#ifdef CONFIG_IMX_BOOTAUX
/* Set to QSPI1 A flash at default */
#define CONFIG_SYS_AUXCORE_BOOTDATA 0x60000000

#define UPDATE_M4_ENV \
	"m4image=m4_qspi.bin\0" \
	"loadm4image=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${m4image}\0" \
	"update_m4_from_sd=" \
		"if sf probe 0:0; then " \
			"if run loadm4image; then " \
				"setexpr fw_sz ${filesize} + 0xffff; " \
				"setexpr fw_sz ${fw_sz} / 0x10000; "	\
				"setexpr fw_sz ${fw_sz} * 0x10000; "	\
				"sf erase 0x0 ${fw_sz}; " \
				"sf write ${loadaddr} 0x0 ${filesize}; " \
			"fi; " \
		"fi\0" \
	"m4boot=sf probe 0:0; bootaux "__stringify(CONFIG_SYS_AUXCORE_BOOTDATA)"\0"
#else
#define UPDATE_M4_ENV ""
#endif

#define MY_CONFIG_NETWORK_SETTINGS \
	"serverip=192.168.3.60\0" \
	"ipaddr=192.168.3.222\0" \
	"ethaddr=02:00:99:31:79:30\0"

/*
 * Use:
 * 		bootmode=mix
 * 		bootmode=mmc
 * 		bootmode=net
 */
#define MY_CONFIG_BOOT_MODE	"boot-mode=mmc\0"

#define CONFIG_EXTRA_ENV_SETTINGS \
	UPDATE_M4_ENV \
	MY_CONFIG_NETWORK_SETTINGS \
	MY_CONFIG_BOOT_MODE \
	"image=zImage\0" \
	"console=ttymxc0\0" \
	"fdt_file=imx7d-cm.dtb\0" \
	"fdt_addr=0x83000000\0" \
	"mmcdev="__stringify(CONFIG_SYS_MMC_ENV_DEV)"\0" \
	"mmcpart=" __stringify(CONFIG_SYS_MMC_IMG_LOAD_PART) "\0" \
	"mmcroot=" CONFIG_MMCROOT " rootwait rw\0" \
	"mmcargs=setenv bootargs console=${console},${baudrate} " \
		"root=${mmcroot}\0" \
		"loadimage=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${image}\0" \
		"loadfdt=fatload mmc ${mmcdev}:${mmcpart} ${fdt_addr} ${fdt_file}\0" \
	\
	"bootmmc=" \
		"echo Booting from SD card ...; " \
		"run mmcargs; " \
		"mmc dev ${mmcdev};" \
		"run loadimage; " \
		"run loadfdt; " \
		"bootz ${loadaddr} - ${fdt_addr}; " \
		"\0" \
	\
	"bootmix=" \
		"echo Boot Kernel and FDT from TFTP, RootFs from SD card ...; " \
		"run mmcargs; " \
		"mmc dev ${mmcdev};" \
		"tftp ${fdt_addr} ${fdt_file}; " \
		"tftp ${image}; " \
		"bootz ${loadaddr} - ${fdt_addr}; " \
		"\0" \
	\
	"netargs=setenv bootargs console=${console},${baudrate} " \
		"root=/dev/nfs " \
		"ip=dhcp nfsroot=${serverip}:${nfsroot},v3,tcp" \
		"\0" \
	"bootnet=" \
		"echo Booting from net ...; " \
		"run netargs; " \
		"tftp ${image}; " \
		"tftp ${fdt_addr} ${fdt_file}; " \
		"bootz ${loadaddr} - ${fdt_addr}; " \
		"\0"

#define CONFIG_BOOTCOMMAND "run boot${boot-mode}"

#define CONFIG_SYS_MEMTEST_START	0x80000000
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_MEMTEST_START + 0x10000000)

#define CONFIG_SYS_LOAD_ADDR		CONFIG_LOADADDR
#define CONFIG_SYS_HZ				1000

/* Physical Memory Map */
#define PHYS_SDRAM					MMDC0_ARB_BASE_ADDR

#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM
#define CONFIG_SYS_INIT_RAM_ADDR	IRAM_BASE_ADDR
#define CONFIG_SYS_INIT_RAM_SIZE	IRAM_SIZE

#define CONFIG_SYS_INIT_SP_OFFSET \
	(CONFIG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CONFIG_SYS_INIT_SP_ADDR \
	(CONFIG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

/* environment organization */
#define CONFIG_ENV_SIZE				SZ_8K

#define CONFIG_ENV_OFFSET			(12 * SZ_64K)
#define CONFIG_SYS_FSL_USDHC_NUM	2

#define CONFIG_SYS_MMC_ENV_DEV		0   /* USDHC1 */
#define CONFIG_SYS_MMC_ENV_PART		0	/* user area */
#define CONFIG_MMCROOT			"/dev/mmcblk0p2"  /* USDHC1 */

/* USB Configs */
#define CONFIG_MXC_USB_PORTSC  (PORT_PTS_UTMI | PORT_PTS_PTW)

#define CONFIG_IMX_THERMAL
#define CONFIG_USBD_HS

#endif	/* __CONFIG_H */
