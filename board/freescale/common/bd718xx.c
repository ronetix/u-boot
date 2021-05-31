// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2021 Ronetix GmbH
 */

#include <common.h>
#include <power/pmic.h>
#include <power/bd71837.h>

/*!
 * @brief Print Rohm PMIC type
 * @param p
 */
void bd718xx_print_type(struct pmic *p)
{
	unsigned int id = 0, otpver = 0;

	pmic_reg_read(p, BD71837_REV, &id);
	pmic_reg_read(p, BD71837_OTPVER, &otpver);

	printf("PMIC: ");

	if ((id == 0xA3) && (otpver == 1))
		printf("BD71837\n");
	else
	if ((id == 0xA1) && (otpver == 0xC1))
		printf("BD71847\n");
	else
	if ((id == 0xA1) && (otpver == 0xC3))
		printf("BD71850\n");
	else
		printf("BD718xx, ID=0x%02x, OTPVER=0x%02X\n", id, otpver);
}
