/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "tedit.h"

int main(int argc, char *argv[])
{
	tedit_start(argc > 1 ? argv+1 : NULL, argc-1);
	return 0;
}
