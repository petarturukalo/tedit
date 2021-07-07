/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Auxiliary commands.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef ACMD_H
#define ACMD_H

#include "cmd.h"
#include "../math.h"
#include "../chrp.h"


/*
 * Command to list all of the open file buffers
 */
extern cmd_t acmd_list;

/*
 * Command to quit the program
 * Requires that the current file be saved/written to before quitting.
 */
extern cmd_t acmd_quit;

/*
 * Command to force quit.
 * The current file buffer doesn't need to be saved before quitting.
 */
extern cmd_t acmd_fquit;

#endif
