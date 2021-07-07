/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Parse an array of commands.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef CPARSE_H
#define CPARSE_H

#include <curses.h>
#include <search.h>
#include "cmd.h"
#include "cmds.h"
#include "../chrp.h"

/**
 * cmds_parse - Parse an array of commands
 */
char *cmds_parse(char *args, cmds_t *cs, bufs_t *b, WINDOW *w);

#endif
