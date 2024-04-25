#
# Copyright 2018 NXP
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += imx8mm_cm.o

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
obj-$(CONFIG_TARGET_IMX8MM_CM) 		   += lpddr4_timing.o
obj-$(CONFIG_TARGET_IMX8MM_COMPACT_CM) += lpddr4_timing_compact.o
endif
