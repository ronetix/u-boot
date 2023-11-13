#!/bin/bash
# SPDX-License-Identifier: GPL-2.0+
#
# i.MX8M and i.MX9: script for automatically download and build:
#   - ARM Trusted firmware
#   - DDR/HDMI firmware
#
# The script set an environment variable (ATF_LOAD_ADDR)
# and because of this it should be sourced.
#
# Ronetix GmbH, iliev@ronetix.at

DRYRUN="no"

this=${BASH_SOURCE[0]}

function usage
{
	echo "Usage:"
	echo "  source $this imx8mq | imx8mm | imx8mn | imx93"
	echo "For example:"
	echo "  source $this imx8mq"
}

# check if the script is sourced or directly executed
if [ "$this" == "${0}" ]; then
	usage
	exit 1
fi

soc=$1
clean=$2

# toolchain can be found here:
# http://download.ronetix.at/toolchains/crosstool-ng
export CROSS_COMPILE=/opt/cross/aarch64-ronetix-elf/bin/aarch64-ronetix-elf-
export ARCH=arm64

ATF_DIR="imx-atf"
ATF_URL="https://github.com/nxp-imx/imx-atf"
MKIMAGE_URL="https://github.com/nxp-imx/imx-mkimage"
IMX_FIRMWARE_URL="https://www.nxp.com/lgfiles/NMG/MAD/YOCTO"
OUTPUT_DIR="."
FLAG_HDMI="n"
FIRMWARE=""
MKIMAGE_BRANCH=""
FIRMWARE_SENTINEL=""
MKIMAGE_DIR=""
MKIMAGE_SOC_DIR=""

#-----------------------------------------------------------------------
#                       run()
#
#   Parameter: Command to be executed
#
#   Description:
#       The argument is a command whcih will be executed
#-----------------------------------------------------------------------
function run()
{
    if [ "$DRYRUN" == "yes" ]; then
		echo DRY: $*
		return 0
	fi

    echo $*
    eval $@

    # check return status
    if [ "$?" != "0" ]; then
		echo "----------------------------------------------------------"
		echo "FAILED: $@"
		echo "----------------------------------------------------------"
		return 1
	fi
}

case "$soc" in
"imx8mq")
	# The imx-atf from Ronetix is modified to run the bl31 at address
	# 0x90000 instead of 0x910000. This is necessary in order to make room
	# for the FDT file.
	ATF_BRANCH="imx_4.19.35_1.0.0"
	FIRMWARE_IMX="firmware-imx-7.9"
	FLAG_HDMI="y"
	ATF_LOAD_ADDR="0x910000"
	run export ATF_LOAD_ADDR=$ATF_LOAD_ADDR
	SEEK=33
	;;

"imx8mm")
	ATF_BRANCH="imx_4.19.35_1.0.0"
	FIRMWARE_IMX="firmware-imx-8.0"
	ATF_LOAD_ADDR=0x920000
	run export ATF_LOAD_ADDR=$ATF_LOAD_ADDR
	SEEK=33
	;;

"imx8mn")
	ATF_BRANCH="imx_4.19.35_1.0.0"
	FIRMWARE_IMX="firmware-imx-8.5"
	ATF_LOAD_ADDR=0x960000
	run export ATF_LOAD_ADDR=$ATF_LOAD_ADDR
	SEEK=32
	;;

"imx93")
	ATF_BRANCH="lf_v2.8"
	FIRMWARE_IMX="firmware-imx-8.21"
	FIRMWARE_SENTINEL="firmware-sentinel-0.11"
	MKIMAGE_BRANCH="lf-6.1.36_2.1.0"
	MKIMAGE_DIR="imx-mkimage"
	MKIMAGE_SOC="iMX9"
	OUTPUT_DIR=$MKIMAGE_DIR/$MKIMAGE_SOC
	;;
*)
	usage
	return
esac

if [ "$clean" == "clean" ]; then
	echo "Remove cloned and downloaded folders and files"
	[ "$MKIMAGE_DIR" != "" ] && run rm -rf $MKIMAGE_DIR
	[ "$FIRMWARE_IMX" != "" ] && run rm -rf $FIRMWARE_IMX*
	[ "$FIRMWARE_SENTINEL" != "" ] && run rm -fr $FIRMWARE_SENTINEL*
	[ "$ATF_DIR" != "" ] && run rm -rf $ATF_DIR
	run rm -f lpddr*.bin
	run rm -f signed_hdmi_*.bin
	run rm -f bl31.bin
	return
fi

echo "Get and Build the ARM Trusted firmware for $soc"

if [ "$MKIMAGE_BRANCH" != "" ]; then
	echo "Get and build IMX mkimage tool"
	if [ ! -d "$MKIMAGE_DIR" ]; then
		run "git clone $MKIMAGE_URL $MKIMAGE_DIR -b $MKIMAGE_BRANCH" || return
	fi
	run cd $MKIMAGE_DIR
	run make SOC=iMX8M mkimage_imx8 || return
	run cd ..
fi

if [ "$FIRMWARE_IMX" != "" ]; then
	if [ ! -d "$ATF_DIR" ]; then
		run "git clone $ATF_URL $ATF_DIR -b $ATF_BRANCH" || return
	fi
	run make -C $ATF_DIR PLAT=$soc bl31 || return
	run cp $ATF_DIR/build/$soc/release/bl31.bin . || return
fi

if [ "$FIRMWARE_IMX" != "" ]; then
	echo "Get the NXP DDR and HDMI firmware"
	if [ ! -f "$FIRMWARE_IMX.bin" ]; then
		run wget $IMX_FIRMWARE_URL/$FIRMWARE_IMX.bin || return
	fi	
	if [ ! -d "$FIRMWARE_IMX" ]; then	
		run chmod +x $FIRMWARE_IMX.bin || return
		run ./$FIRMWARE_IMX.bin || return
	fi

	run cp $FIRMWARE_IMX/firmware/ddr/synopsys/lpddr4*.bin $OUTPUT_DIR || return

	if [ "FLAG_HDMI" == "y" ]; then
		run cp $FIRMWARE_IMX/firmware/hdmi/cadence/signed_hdmi_$soc.bin . || return
	fi
fi

if [ "$FIRMWARE_SENTINEL" != "" ]; then
	echo "Get Sentinel firmware"
	if [ ! -f "$FIRMWARE_SENTINEL.bin" ]; then
		run wget $IMX_FIRMWARE_URL/$FIRMWARE_SENTINEL.bin || return
	fi
	if [ ! -d "$FIRMWARE_SENTINEL" ]; then	
		run chmod +x $FIRMWARE_SENTINEL.bin || return
		run ./$FIRMWARE_SENTINEL.bin || return
	fi
	run cp $FIRMWARE_SENTINEL/mx93*.img $OUTPUT_DIR || return
fi

if [ "$MKIMAGE_BRANCH" != "" ]; then
	run cp $ATF_DIR/build/imx93/release/bl31.bin $OUTPUT_DIR || return

	run cp u-boot.bin $OUTPUT_DIR || return
	run cp spl/u-boot-spl.bin $OUTPUT_DIR || return
	run cd $MKIMAGE_DIR
	run make SOC=$MKIMAGE_SOC flash_singleboot || return
	run cd ..
	echo "The result is: $OUTPUT_DIR/flash.bin"
fi

echo "done."
