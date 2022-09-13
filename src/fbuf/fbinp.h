/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * File buffer input. Have a key press affect a file buffer.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef FBINP_H
#define FBINP_H

#include "bufs.h"
#include "fbuf.h"
#include "../move.h"
#include "../linsert.h"
#include "../tedata.h"
#include "../getch.h"

/*
 * fbinp_handle_char - Handle having read a character 
 *
 * Have the read character affect the file buffer by potentially
 * updating the cursor and adding/removing a character.
 */
void fbinp_handle_char(bufs_t *b, int c);

#endif
