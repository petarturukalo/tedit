/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#include "colour.h"

void init_pairs(void)
{
	use_default_colors();

	init_pair(COLOUR_DEFAULT, -1,            -1);
	init_pair(COLOUR_BLACK,   COLOR_BLACK,   -1);
	init_pair(COLOUR_RED,     COLOR_RED,     -1);
	init_pair(COLOUR_GREEN,   COLOR_GREEN,   -1);
	init_pair(COLOUR_YELLOW,  COLOR_YELLOW,  -1);
	init_pair(COLOUR_BLUE,    COLOR_BLUE,    -1);
	init_pair(COLOUR_MAGENTA, COLOR_MAGENTA, -1);
	init_pair(COLOUR_CYAN,    COLOR_CYAN,    -1);
	init_pair(COLOUR_WHITE,   COLOR_WHITE,   -1);
}
