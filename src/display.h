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

/**
 * display_start - Start the display part of the text editor
 *
 * Starts the curses screen to show the text editor and editing file.
 */
void display_start(tedata_t *t);

#endif

