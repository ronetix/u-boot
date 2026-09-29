#
# Copyright 2018 NXP
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += imx8mm_cm.o

ifdef CONFIG_XPL_BUILD
obj-y += spl.o
obj-$(CONFIG_TARGET_IMX8MM_COMPACT_CM) += lpddr4_timing_1gb.o
obj-$(CONFIG_TARGET_IMX8MM_COMPACT_2GB_CM) += lpddr4_timing_2gb.o
endif
