/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Template for commands that can be ran by users.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef CMD_H
#define CMD_H

#include <curses.h>
#include "../fbuf/bufs.h"

struct command {
	char *short_name;  // Unique few character identifier for the command.
	char *long_name;  // Unique longer string identifier for command.
	// Function to run command. First parameter is remaining part of the command string that user
	// entered (passed the command name). An output parameter is the cmd_ostr field in bufs_t
	// and is used to set the echo message.
	void (*handler)(char *, bufs_t *, WINDOW *);
};

typedef struct command cmd_t;

#endif
