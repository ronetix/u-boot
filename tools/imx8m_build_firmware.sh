#!/bin/bash
# SPDX-License-Identifier: GPL-2.0+
#
# imx8m_build_firmware.sh - prepare the firmware needed to build flash.bin
#                           for i.MX8M (Quad, Mini, Nano) boards
#
# Copyright (C) Ronetix GmbH, Ilko Iliev <iliev@ronetix.at>
#
# Usage (from the U-Boot top directory):
#
#   make <board>_defconfig
#   tools/imx8m_build_firmware.sh <soc>
#   make
#
#   <soc> = imx8mq | imx8mm | imx8mn
#
# Example:
#
#   make imx8mn_compact_cm_defconfig
#   tools/imx8m_build_firmware.sh imx8mn
#   make
#
# What it does:
#
#   1. Clones the ARM Trusted Firmware into ./imx-atf (only if missing),
#      builds BL31 for <soc> and copies it to ./bl31.bin.
#        imx8mm, imx8mn: https://github.com/nxp-imx/imx-atf, lf_v2.10
#        imx8mq:         https://github.com/ronetix/imx-atf, imx_4.19.35_1.0.0
#
#   2. Downloads and extracts the NXP firmware package (only if missing)
#      and copies the LPDDR4 training firmware (lpddr4_pmu_train_*.bin),
#      plus the HDMI firmware for imx8mq, to the current directory.
#        imx8mm, imx8mn: firmware-imx-8.20
#        imx8mq:         firmware-imx-7.9
#
#   binman picks up these files when "make" builds flash.bin
#   ("make flash.bin" is not needed anymore, but still works).
#
# Notes:
#
#   - Run the script again after switching to a board with a different
#     SoC, otherwise flash.bin gets the BL31 built for the previous SoC.
#   - Export CROSS_COMPILE before running the script, it is used to build
#     the ATF.
#   - The NXP firmware license (EULA) is accepted automatically.
#   - The BL31 load address is fixed in the binman description
#     (arch/arm/dts/imx8m*-u-boot.dtsi), so no environment variable
#     (the former ATF_LOAD_ADDR) has to be exported. The script can be
#     executed directly; sourcing it still works.

usage()
{
	echo "Usage:"
	echo "  $0 imx8mq | imx8mm | imx8mn"
	echo "For example:"
	echo "  $0 imx8mn"
}

run_cmd()
{
	echo "$@"
	eval "$@"
}

main()
{
	local soc=$1
	local FLAG_HDMI="n"
	local SRC_URI BRANCH_ATF FIRMWARE FIRMWARE_BIN

	case "$soc" in
	"imx8mq")
		# The imx-atf from Ronetix is modified to run the bl31 at address
		# 0x90000 instead of 0x910000. This is necessary in order to make room
		# for the FDT file.
		SRC_URI="https://github.com/ronetix/imx-atf"
		BRANCH_ATF="imx_4.19.35_1.0.0"
		FIRMWARE="firmware-imx-7.9"
		FLAG_HDMI="y"
		;;

	"imx8mm" | "imx8mn")
		SRC_URI="https://github.com/nxp-imx/imx-atf"
		BRANCH_ATF="lf_v2.10"
		FIRMWARE="firmware-imx-8.20"
		;;

	*)
		usage
		return 1
	esac

	FIRMWARE_BIN=$FIRMWARE.bin

	echo "Get and Build the ARM Trusted firmware for $soc"

	if [ ! -d "imx-atf" ]; then
		run_cmd "git clone -b $BRANCH_ATF $SRC_URI imx-atf" || return
	fi

	run_cmd "(cd imx-atf && make PLAT=$soc bl31)" || return
	run_cmd cp imx-atf/build/$soc/release/bl31.bin . || return

	echo "Get the NXP DDR and HDMI firmware"
	if [ ! -d "$FIRMWARE" ]; then
		run_cmd rm -rf firmware-imx-*
		run_cmd wget https://www.nxp.com/lgfiles/NMG/MAD/YOCTO/$FIRMWARE_BIN || return
		run_cmd chmod +x $FIRMWARE_BIN
		run_cmd ./$FIRMWARE_BIN --auto-accept || return
	fi
	run_cmd cp $FIRMWARE/firmware/ddr/synopsys/lpddr4_pmu_train_*.bin . || return

	if [ "$FLAG_HDMI" == "y" ]; then
		run_cmd cp $FIRMWARE/firmware/hdmi/cadence/signed_hdmi_$soc.bin . || return
	fi

	echo "done."
}

# Works both executed and sourced (the old way), without closing the
# calling shell on error.
main "$@"
_rc=$?
return $_rc 2>/dev/null || exit $_rc
