/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Math related functions.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef MATH_H
#define MATH_H

#include <math.h>

/**
 * round_up_pow2 - Round a number up to the nearest power of 2 
 *
 * Example: rounding 6 up to the nearest power of 2 would round it up 
 * to 8 (2^3), rounding 8 stays at 8, and rounding 9 rounds up to 16 (2^4), etc..
 */
int round_up_pow2(int n);

/**
 * ndigits - Get the amount of digits in a number (base 10 decimal)
 */
int ndigits(int nr);

#endif
