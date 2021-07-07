/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Template for commands that can be ran by users.
 * Implement a command by making a function which returns a dynamically
 * allocated command struct.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef CMD_H
#define CMD_H

#include <curses.h>
#include "../fbuf/bufs.h"

struct command {
	char *c_short_name;  // Unique few character identifier for the command.
	char *c_long_name;  // Unique longer string identifier for command.
	// Function to run command. First parameter is remaining part of string that user
	// entered (not the command name). Return is an error/success message which is
	// echoed back to the echo line.
	char *(*c_handler)(char *, bufs_t *, WINDOW *);
};

typedef struct command cmd_t;


/**
 * cmd_init - Initialise a command struct
 *
 * See struct command fields for parameter info.
 * Dynamically allocated. Return NULL on error. Must be freed with free.
 */
cmd_t *cmd_init(char *short_name, char *long_name, char *(*handler)(char *, bufs_t *, WINDOW *));

#endif
