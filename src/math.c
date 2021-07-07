/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "math.h"

int round_up_pow2(int n)
{
	int res;
	int exp = ceil(log2(n));

	res = pow(2, exp);

	if (res == 0)
		return 1;
	return res;
}

int ndigits(int nr)
{
	return (nr/10)+1;
}

