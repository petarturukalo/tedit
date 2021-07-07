/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Signal handling.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef SIG_H
#define SIG_H

#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "tedata.h"

/**
 * sig_clean_exit - Signal handler to clean up and free all initialised data before
 *	exiting
 */
void sig_clean_exit(int sig);

#endif
