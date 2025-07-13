#
# Copyright 2022 NXP
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += imx93_osm.o

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
obj-$(CONFIG_IMX93_OSM_LPDDR4X_1GB) += lpddr4x_timing.o lpddr4x_timing_1866mts.o
obj-$(CONFIG_IMX93_OSM_LPDDR4X_2GB) += lpddr4x_timing_2gb.o lpddr4x_timing_1866mts_2gb.o
endif
