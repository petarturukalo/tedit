/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <curses.h>
#include "tedata.h"
#include "fbuf/fbaux.h"

// Refresh rates in micro seconds.
#define REFRESH_RATE_60_HZ_USEC 16667
#define REFRESH_RATE_120_HZ_USEC 8333
#define REFRESH_RATE_USE_USEC REFRESH_RATE_60_HZ_USEC  // Refresh rate that gets used.

/*
 * Display the text editor to the curses standard screen.
 */
void display_text_editor(tedata_t *t);

#endif
