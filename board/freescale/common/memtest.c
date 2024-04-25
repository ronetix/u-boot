// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2021 Ronetix GmbH
 */

#include <common.h>
#include "memtest.h"

#define TEST_PATTERN		0x123456789AB00000

/*!
 * @brief Make a simple memory test
 * @param start - start address
 * @param len - length in bytes
 * @param pattern
 * @return
 */
static uint64_t mem_block_test(uint64_t start, int len)
{
	int i;
	uint64_t *p;
	uint64_t val;

	len = len / (sizeof(uint64_t));

	p = (uint64_t *)start;
	val = TEST_PATTERN;

	/* fill the memory block with test pattern */
	for (i = 0; i < len; i++)
		*p++ = val++;

	p = (uint64_t *)start;
	val = TEST_PATTERN;

	/* read back and check */
	for (i = 0; i < len; i++)
	{
		if (*p != val++)
			return (uint64_t)p;

		p++;
	}

	return 0;
}

/*!
 * @brief Fast simple memory test
 *	Tests only 'test_size'of every block
 *
 * @param addr - start address
 * @param len - length in bytes
 * @param block_size - length of a block in bytes
 * @param test_size - test length in bytes
 * @return
 */
int mem_test(uint64_t addr, int len, int block_size, int test_size)
{
	int i;

	addr &= ~7;

	printf("Fast testing %d MiB @ 0x%lx ... ", len / 1024 / 1024, (unsigned long)addr);

	for (i = 0; i < len / block_size; i++)
	{
		uint64_t ret;

		ret = mem_block_test(addr, test_size);
		if (ret)
		{
			printf("failed at 0x%lx\n", (unsigned long)ret);
			return 1;
		}

		addr += block_size;
	}

	printf("OK\n");

	return 0;
}
