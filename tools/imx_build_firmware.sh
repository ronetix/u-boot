#!/bin/bash
# SPDX-License-Identifier: GPL-2.0+
#
# imx_build_firmware.sh - prepare the firmware needed to build flash.bin
#                         for i.MX8M (Quad, Mini, Nano) and i.MX93 boards
#
# Copyright (C) Ronetix GmbH, Ilko Iliev <iliev@ronetix.at>
#
# Usage (from the U-Boot top directory):
#
#   make <board>_defconfig
#   tools/imx_build_firmware.sh <soc>
#   make
#
#   <soc> = imx8mq | imx8mm | imx8mn | imx93
#
#   CPU_REV=A0 tools/imx_build_firmware.sh imx93
#
#   selects the ELE container for the i.MX93 CPU revision, A0, A1 or A2
#   (A1 is the default). A0 uses mx93a0-ahab-container.img, A1 and A2 both
#   use mx93a1-ahab-container.img. Run "make" again afterwards to rebuild
#   flash.bin. The RNX-iMX93-OSM carries an A0 die, so it needs CPU_REV=A0.
#
#   tools/imx_build_firmware.sh clean
#
#   removes everything this script downloaded or built, for all SoCs. This
#   includes local changes in the ATF clones. Afterwards run the script
#   again before building flash.bin.
#
# Example:
#
#   make imx8mn_compact_cm_defconfig
#   tools/imx_build_firmware.sh imx8mn
#   make
#
# What it does:
#
#   1. Clones the ARM Trusted Firmware (only if missing), builds BL31 for
#      <soc> and copies it to ./bl31.bin.
#        imx8mm, imx8mn: https://github.com/nxp-imx/imx-atf, lf_v2.10
#                        into ./imx-atf
#        imx8mq:         https://github.com/ronetix/imx-atf, imx_4.19.35_1.0.0
#                        into ./imx-atf
#        imx93:          https://github.com/nxp-imx/imx-atf, lf_v2.14
#                        into ./imx-atf-imx93
#
#   2. Downloads and extracts the NXP firmware packages (only if missing) and
#      copies the needed files to the current directory:
#        imx8mm, imx8mn: firmware-imx-8.20, LPDDR4 training firmware
#        imx8mq:         firmware-imx-7.9, LPDDR4 training and HDMI firmware
#        imx93:          firmware-imx-8.32-1991416, LPDDR4 training firmware
#                        firmware-ele-imx-2.0.6-c0b284c, ELE container
#                        mx93a1-ahab-container.img for CPU_REV=A1 and A2,
#                        or firmware-ele-imx-0.1.0 and
#                        mx93a0-ahab-container.img for CPU_REV=A0 (later
#                        packages no longer ship the A0 container),
#                        copied to mx93-ahab-container.img
#
#      These are the versions meta-freescale uses, so a standalone flash.bin
#      matches the one the Yocto BSP builds: imx-atf lf_v2.14 (imx-atf_2.14.bb),
#      firmware-imx 8.32 and firmware-ele-imx 2.0.6. Keeping them in step
#      matters - the older ELE packages each carry a different
#      mx93a1-ahab-container.img.
#
#   binman picks up these files when "make" builds flash.bin.
#   On i.MX8M "make flash.bin" still works. On i.MX93 it fails with
#   "No rule to make target 'u-boot.cnt'", so use "make".
#
# Notes:
#
#   - Run the script again after switching to a board with a different
#     SoC, otherwise flash.bin gets the BL31 built for the previous SoC.
#   - Export CROSS_COMPILE before running the script, it is used to build
#     the ATF.
#   - mx93-ahab-container.img is used by the Ronetix i.MX93 boards
#     (board/ronetix/imx93-osm/imximage.cfg). A flash.bin with the
#     container of the wrong CPU revision does not boot.
#   - The NXP firmware license (EULA) is accepted automatically.
#   - The BL31 load address is fixed in the binman description
#     (arch/arm/dts/imx8m*-u-boot.dtsi, arch/arm/mach-imx/imx9/container.cfg),
#     so no environment variable (the former ATF_LOAD_ADDR) has to be
#     exported. The script can be executed directly; sourcing it still works.

FIRMWARE_URI="https://www.nxp.com/lgfiles/NMG/MAD/YOCTO"

usage()
{
	echo "Usage:"
	echo "  $0 imx8mq | imx8mm | imx8mn | imx93 | clean"
	echo "  CPU_REV=A0|A1|A2 $0 imx93   (i.MX93 CPU revision, default A1)"
	echo "For example:"
	echo "  $0 imx8mn"
}

run_cmd()
{
	echo "$@"
	eval "$@"
}

get_firmware()
{
	local name=$1

	if [ ! -d "$name" ]; then
		run_cmd wget -N $FIRMWARE_URI/$name.bin || return
		run_cmd chmod +x $name.bin || return
		run_cmd ./$name.bin --auto-accept || return
	fi
}

clean()
{
	# The paths below are relative, do not delete files of another directory
	if [ ! -f tools/imx_build_firmware.sh ]; then
		echo "Error: run \"clean\" from the U-Boot top directory"
		return 1
	fi

	run_cmd rm -rf imx-atf imx-atf-imx93 firmware-imx-* \
		firmware-sentinel-* firmware-ele-imx-*
	run_cmd rm -f bl31.bin lpddr4*.bin signed_hdmi_*.bin mx93*-ahab-container.img

	echo "done."
}

main()
{
	local soc=$1
	local FLAG_HDMI="n"
	local ATF_DIR="imx-atf"
	local FIRMWARE_SENTINEL=""
	local SRC_URI BRANCH_ATF FIRMWARE
	local cpu_rev=${CPU_REV:-A1}
	local ele_rev ele_pkg

	# The ELE container is not named after every CPU revision: an A0 die
	# needs mx93a0-ahab-container.img, while A1 and A2 both run the A1
	# container. And the packages differ - mx93a0 was dropped after
	# firmware-ele-imx-0.1.0, the package the 6.1 BSP used, so A0 has to
	# come from there rather than from the current firmware-sentinel.
	case "$cpu_rev" in
	"A0")
		ele_rev="a0"
		ele_pkg="firmware-ele-imx-0.1.0"
		;;
	"A1" | "A2")
		ele_rev="a1"
		ele_pkg="firmware-ele-imx-2.0.6-c0b284c"
		;;
	*)
		echo "Error: CPU_REV=$cpu_rev, must be A0, A1 or A2"
		return 1
	esac

	case "$soc" in
	"clean")
		clean
		return
		;;

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

	"imx93")
		SRC_URI="https://github.com/nxp-imx/imx-atf"
		BRANCH_ATF="lf_v2.14"
		ATF_DIR="imx-atf-imx93"
		FIRMWARE="firmware-imx-8.32-1991416"
		FIRMWARE_SENTINEL="$ele_pkg"
		;;

	*)
		usage
		return 1
	esac

	echo "Get and Build the ARM Trusted firmware for $soc"

	if [ ! -d "$ATF_DIR" ]; then
		run_cmd "git clone -b $BRANCH_ATF $SRC_URI $ATF_DIR" || return
	else
		# A clone from an earlier run is reused as it is, so a changed
		# BRANCH_ATF would otherwise silently keep building the old
		# branch. That bit us when imx93 moved from lf_v2.8 to lf_v2.14.
		local have_branch
		have_branch=$(git -C "$ATF_DIR" rev-parse --abbrev-ref HEAD 2>/dev/null)
		if [ "$have_branch" != "$BRANCH_ATF" ]; then
			echo "Error: $ATF_DIR is on branch '$have_branch'," \
			     "expected '$BRANCH_ATF'."
			echo "       Remove it (or run '$0 clean') and try again."
			return 1
		fi
	fi

	run_cmd "(cd $ATF_DIR && unset LDFLAGS && make PLAT=$soc bl31)" || return
	run_cmd cp $ATF_DIR/build/$soc/release/bl31.bin . || return

	echo "Get the NXP DDR and HDMI firmware"
	get_firmware $FIRMWARE || return

	if [ "$soc" == "imx93" ]; then
		run_cmd cp $FIRMWARE/firmware/ddr/synopsys/lpddr4*.bin . || return
	else
		run_cmd cp $FIRMWARE/firmware/ddr/synopsys/lpddr4_pmu_train_*.bin . || return
	fi

	if [ "$FLAG_HDMI" == "y" ]; then
		run_cmd cp $FIRMWARE/firmware/hdmi/cadence/signed_hdmi_$soc.bin . || return
	fi

	if [ -n "$FIRMWARE_SENTINEL" ]; then
		echo "Get the NXP ELE firmware for CPU revision $cpu_rev" \
		     "(mx93$ele_rev container from $FIRMWARE_SENTINEL)"
		get_firmware $FIRMWARE_SENTINEL || return
		run_cmd cp $FIRMWARE_SENTINEL/mx93$ele_rev-ahab-container.img \
			mx93-ahab-container.img || return

		# Force the SPL container description to be regenerated.
		#
		# The "APPEND mx93-ahab-container.img" of CONFIG_IMX_CONFIG is
		# preprocessed into spl/u-boot-spl.cfgout, and it ends up
		# commented out there when the container was not present at the
		# time that file was generated. Nothing makes the cfgout depend
		# on the container, so a "make" run before this script leaves a
		# stale cfgout behind and the next "make" silently builds a
		# flash.bin without the ELE firmware. It boots, but ELE never
		# starts and U-Boot reports
		#   BuildInfo:
		#     - ELE firmware not included
		# instead of the version. Removing these makes the order of
		# "make" and this script irrelevant.
		run_cmd rm -f spl/u-boot-spl.cfgout spl.bin flash.bin .binman_stamp
	fi

	echo "done."
}

# Works both executed and sourced (the old way), without closing the
# calling shell on error.
main "$@"
_rc=$?
return $_rc 2>/dev/null || exit $_rc
