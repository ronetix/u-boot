/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2021 Ronetix GmbH
 */

#ifndef __MEMTEST_BOARD_HELPER__
#define __MEMTEST_BOARD_HELPER__

int mem_test(uint64_t start, int len, int block_size, int test_size);

#endif

