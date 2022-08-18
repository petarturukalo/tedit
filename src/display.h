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

void display_start(tedata_t *t);

#endif
