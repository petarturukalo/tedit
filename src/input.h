/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef INPUT_H
#define INPUT_H

#include "tedata.h"
#include "fbuf/fbuf.h"
#include "fbuf/fbinp.h"
#include "fbuf/elinp.h"
#include "getch.h"

/*
 * Start the user input main loop. Each loop a key is read from the user
 * and that key handled by having it affect the current file buffer.
 */
void input_start(tedata_t *t);

#endif
