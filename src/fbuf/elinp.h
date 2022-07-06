/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Handle input to an echo line file buffer.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef ELINP_H
#define ELINP_H

#include "elbuf.h"
#include "fbuf.h"
#include "fbinp.h"
#include "bufs.h"
#include "../cmd/cparse.h"

/*
 * elinp_handle_char - Handle having read a character
 * @c: character read
 *
 * This is a restricted wrapper for fbinp_handle_char.
 */
void elinp_handle_char(bufs_t *b, int c, cmds_t *cs, WINDOW *w);

#endif
