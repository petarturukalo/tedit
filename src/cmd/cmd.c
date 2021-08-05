/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "cmd.h"

cmd_t *cmd_init(char *short_name, char *long_name, char *(*handler)(char *, bufs_t *, WINDOW *))
{
	cmd_t *c = malloc(sizeof(cmd_t));

	if (!c)
		return NULL;

	c->short_name = short_name;
	c->long_name = long_name;
	c->handler = handler;

	return c;
}
