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
#include <string.h>
#include "cmd.h"
#include "../chrp.h"

/*
 * cmds_parse - Parse and run a command
 * @args: arguments containing command name and input arguments for the command
 * 
 * Sets cmd_ostr in bufs_t as out-parameter, a message describing the result of
 * the command, or an error message.
 */
void cmds_parse(char *args, cmds_t *cs, bufs_t *b, WINDOW *w);

#endif
