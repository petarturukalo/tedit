/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "cparse.h"

/*
 * args_parse_remain_args - Get the arguments part of an argument string (which includes command name),
 *	with leading whitespace and trailing whitespace removed
 */
char *args_parse_remain_args(char *args)
{
	int i, len1, len2, n;

	i = 0;
	n = strlen(args);

	// Push to just after command name and start of leading 
	// whitespace of remaining arguments.
	while (i < n && args[i] == ' ')
		++i;
	while (i < n && args[i] != ' ')
		++i;

	if (i == -1) 
		return NULL;
	// Length of whitespace gap between command name and arguments.
	len1 = chrp_nmatched(args, ' ', i, n-1);

	// Whitespace stretches to the end of the string.
	if (i+len1 == n)
		return NULL;

	// Know there is at least one character that isn't whitespace as the 
	// second argument.
	len2 = chrp_nmatched_reverse(args, ' ', 0, n-1);

	if (len2 > 0) 
		args[n-len2] = '\0';
	return args+i+len1;
}

/*
 * args_parse_cmdname - Get the command name part of an argument string
 *
 * Call this after args_parse_remain_args since it possibly sets a null termination character
 * early on.
 */
char *args_parse_cmdname(char *args)
{
	int i, j, n;

	i = 0;
	n = strlen(args);

	while (i < n && args[i] == ' ')
		++i;

	j = chrp_find(args, ' ', i, n-1);

	if (j == -1)
		return args+i;
	args[j] = '\0';
	return args+i;
}

void cmds_parse_aux(char *args, char *cmd_name, char *remain_args, cmds_t *cs, bufs_t *b, WINDOW *w)
{
	cmd_t *c = cmds_search(cs, cmd_name);

	if (c) 
		c->handler(remain_args, b, w);
	else
		strcpy(b->cmd_ostr, "invalid command");
}

void cmds_parse(char *args, cmds_t *cs, bufs_t *b, WINDOW *w)
{
	char *cmd_name, *remain_args;

	remain_args = args_parse_remain_args(args);
	cmd_name = args_parse_cmdname(args);
	
	if (cmd_name) 
		cmds_parse_aux(args, cmd_name, remain_args, cs, b, w);
	else
		strcpy(b->cmd_ostr, "no command name given");
}

