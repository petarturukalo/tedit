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
 * input_start - Start waiting and reading input for use by the text editor
 */
void input_start(tedata_t *t);

#endif
