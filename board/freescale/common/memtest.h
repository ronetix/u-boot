/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2021 Ronetix GmbH
 */

#ifndef __MEMTEST_BOARD_HELPER__
#define __MEMTEST_BOARD_HELPER__

int mem_test(uint64_t start, uint32_t len, uint32_t block_size, uint32_t test_size);

#endif

