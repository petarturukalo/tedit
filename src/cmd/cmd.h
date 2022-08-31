/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Commands ran by the user.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef CMD_H
#define CMD_H

#define _GNU_SOURCE
#include <search.h>
#include <curses.h>
#include "cmd.h"
#include "../fbuf/bufs.h"

typedef struct command {
	char *short_name;  // Unique few character identifier for the command.
	char *long_name;  // Unique longer string identifier for command.
	// Function to run command. First parameter is remaining part of the command string that user
	// entered (passed the command name). An output parameter is the cmd_ostr field in bufs_t
	// and is used to set the echo message.
	void (*handler)(char *, bufs_t *, WINDOW *);
} cmd_t;

// Commands. 
/* List all open file buffers. */
extern cmd_t acmd_list;
/* Quit the program. Requires that the current file be saved/written to before quitting. */
extern cmd_t acmd_quit;
/* Force quit. The current file buffer doesn't need to be saved before quitting. */
extern cmd_t acmd_fquit;
/* Write active file buffer to its linked file or a new file. */
extern cmd_t fcmd_write;
/* Close the active file buffer. File buffer must not have any unsaved edits. */
extern cmd_t fcmd_close;
/* Force close the active file buffer. File buffer doesn't need to be saved. */
extern cmd_t fcmd_fclose;
/* Open a file into a new file buffer. */
extern cmd_t fcmd_open;
/* Edit an existing file buffer by name. */
extern cmd_t fcmd_edit;
/* Jump to a file buffer by ID. Helpful to edit unnamed file buffers. */
extern cmd_t fcmd_jump;

typedef struct commands {
	// Hash table of cmd_t for O(1) lookup of a command.
	struct hsearch_data *htbl;  
} cmds_t;


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
