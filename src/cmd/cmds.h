/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * A collection of commands for parsing (see cparse.h).
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef CMDS_H
#define CMDS_H

#define _GNU_SOURCE
#include <search.h>
#include "cmd.h"
#include "fcmd.h"
#include "acmd.h"

struct commands {
	struct hsearch_data *htbl;  // Hash table for O(1) lookup of a command.
};

typedef struct commands cmds_t;

/*
 * cmds_init - Initialise commands data structure for look up of a command
 * @cs: out-param commands to initialise
 *
 * Free with cmds_free(). 
 */
void cmds_init(cmds_t *cs);

/*
 * cmds_search - Look up a command by name (either short or long name)
 *
 * Return NULL if the command wasn't found.
 */
cmd_t *cmds_search(cmds_t *cs, char *name);

void cmds_free(cmds_t *c);

#endif
