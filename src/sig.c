/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "sig.h"

void sig_clean_exit(int sig)
{
	exit(EXIT_SUCCESS);
}

