/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "cparse.h"

/*
 * Split a string of space separated arguments of format "cmdname [args ...]" into two strings
 * "cmdname" and "[args ...]". Removes any leading and trailing whitespace.
 */
static void split_args(char *args, char **out_cmdname, char **out_remain_args)
{
	*out_remain_args = NULL;

	if ((*out_cmdname = strnchr(args, ' ')) &&
	    (*out_remain_args = strchr(*out_cmdname, ' '))) {
		**out_remain_args = '\0';  // Mark end of command name.
		if (*out_remain_args = strnchr(*out_remain_args+1, ' '))
			strip_trailchar(*out_remain_args, ' ');
	}
}

void cmds_parse(char *args, cmds_t *cs, bufs_t *b, WINDOW *w)
{
	char *cmd_name, *remain_args;
	cmd_t *c;

	split_args(args, &cmd_name, &remain_args);
	
	if (cmd_name) {
		if (c = cmds_search(cs, cmd_name))
			c->handler(remain_args, b, w);
		else
			strcpy(b->cmd_ostr, "invalid command");
	} else
		strcpy(b->cmd_ostr, "no command name given");
}

