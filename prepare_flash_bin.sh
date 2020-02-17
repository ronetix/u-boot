echo "Get and Build the ARM Trusted firmware"

# The imx-atf from Ronetix is modified to run the bl31 at address
# 0x90000 instead of 0x910000. This is necessary in order to make room
# for the FDT file.
git clone https://github.com/ronetix/imx-atf
cd imx-atf
make PLAT=imx8mq bl31
cd ..
cp imx-atf/build/imx8mq/release/bl31.bin .

echo "Get the NXP DDR and HDMI firmware"
rm -f firmware-imx-7.9.bin*
# wget https://www.nxp.com/lgfiles/NMG/MAD/YOCTO/firmware-imx-7.9.bin
wget http://download.ronetix.info/boards/i.MX8MQ-CM/nxp_ddr_hdmi_firmware/firmware-imx-7.9.bin
chmod +x firmware-imx-7.9.bin
./firmware-imx-7.9.bin
cp firmware-imx-7.9/firmware/hdmi/cadence/signed_hdmi_imx8m.bin .
cp firmware-imx-7.9/firmware/ddr/synopsys/lpddr4*.bin .

echo "done."
