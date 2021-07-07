/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "cmds.h"

#define NCMDS 9

void cmds_free(cmds_t *c)
{
	hdestroy_r(c->htbl);
	free(c->htbl);
	free(c);
}

cmd_t *cmds_search(cmds_t *c, char *name)
{
	ENTRY e, *ep;

	e.key = name;

	if (hsearch_r(e, FIND, &ep, c->htbl))
		return (cmd_t *)ep->data;
	return NULL;
}

cmds_t *cmds_init(void)
{
	ENTRY e, *ep;
	cmd_t *c;
	cmds_t *cs;
	cmd_t *CMDS[] = {
		&fcmd_write, &fcmd_close, &fcmd_fclose, &fcmd_open, &fcmd_edit, &acmd_list, 
		&fcmd_jump, &acmd_quit, &acmd_fquit
	};

	cs = malloc(sizeof(cmds_t));
	cs->htbl = malloc(sizeof(struct hsearch_data));

	memset((void *)cs->htbl, 0, sizeof(struct hsearch_data));
	hcreate_r(64, cs->htbl);

	for (int i = 0; i < NCMDS; ++i) {
		c = CMDS[i];

		// A command can be accessed by either short or long name.
		e.data = (void *)c;
		e.key = c->c_short_name;
		hsearch_r(e, ENTER, &ep, cs->htbl);
		e.key = c->c_long_name;
		hsearch_r(e, ENTER, &ep, cs->htbl);
	}
	return cs;
}

