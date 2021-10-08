#!/bin/bash
# SPDX-License-Identifier: GPL-2.0+
#
# i.MX8M: script for automatically download and build:
#   - ARM Trusted firmware
#   - DDR/HDMI firmware
#
# The script set an environment variable (ATF_LOAD_ADDR)
# and because of this it should be sourced.
#
# Ronetix GmbH, iliev@ronetix.at

this=${BASH_SOURCE[0]}

function usage
{
	echo "Usage:"
	echo "  source $this imx8mq | imx8mm | imx8mn"
	echo "For example:"
	echo "  source $this imx8mq"
}

# check if the script is sourced or directly executed
if [ "$this" == "${0}" ]; then
	usage
	exit 1
fi

echo "OK"

soc=$1

FLAG_HDMI="n"

case "$soc" in
"imx8mq")
	# The imx-atf from Ronetix is modified to run the bl31 at address
	# 0x90000 instead of 0x910000. This is necessary in order to make room
	# for the FDT file.
	SRC_URI="https://github.com/ronetix/imx-atf"
	BRANCH_ATF="imx_4.19.35_1.0.0"
	FIRMWARE="firmware-imx-7.9"
	FLAG_HDMI="y"
	ATF_LOAD_ADDR="0x910000"
	SEEK=33
	;;

"imx8mm")
	SRC_URI="https://source.codeaurora.org/external/imx/imx-atf"
	BRANCH_ATF="imx_4.19.35_1.0.0"
	FIRMWARE="firmware-imx-8.0"
	ATF_LOAD_ADDR=0x920000
	SEEK=33
	;;

"imx8mn")
	SRC_URI="https://source.codeaurora.org/external/imx/imx-atf"
	BRANCH_ATF="imx_4.19.35_1.0.0"
	FIRMWARE="firmware-imx-8.5"
	ATF_LOAD_ADDR=0x960000
	SEEK=32
	;;

*)
	usage
	return
esac

FIRMWARE_BIN=$FIRMWARE.bin

echo "Get and Build the ARM Trusted firmware for $soc"

run_cmd()
{
	echo $@
	eval $@
}

if [ ! -d "imx-atf" ]; then
	run_cmd "git clone $SRC_URI"
fi

run_cmd cd imx-atf
run_cmd make distclean
run_cmd make PLAT=$soc bl31
run_cmd cd ..
run_cmd cp imx-atf/build/$soc/release/bl31.bin .

echo "Get the NXP DDR and HDMI firmware"
run_cmd rm -rf firmware-imx-*
run_cmd wget https://www.nxp.com/lgfiles/NMG/MAD/YOCTO/$FIRMWARE_BIN
run_cmd chmod +x $FIRMWARE_BIN
run_cmd ./$FIRMWARE_BIN
run_cmd cp $FIRMWARE/firmware/ddr/synopsys/lpddr4*.bin .

if [ "FLAG_HDMI" == "y" ]; then
	run_cmd cp $FIRMWARE/firmware/hdmi/cadence/signed_hdmi_$soc.bin .
fi

run_cmd export ATF_LOAD_ADDR=$ATF_LOAD_ADDR

echo "done."
