/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "cmds.h"

void cmds_free(cmds_t *c)
{
	hdestroy_r(c->htbl);
	free(c->htbl);
}

cmd_t *cmds_search(cmds_t *c, char *name)
{
	ENTRY e, *ep;

	e.key = name;

	if (hsearch_r(e, FIND, &ep, c->htbl))
		return (cmd_t *)ep->data;
	return NULL;
}

void cmds_init(cmds_t *cs)
{
	ENTRY e, *ep;
	cmd_t *c;
	cmd_t *CMDS[] = {
		&fcmd_write, &fcmd_close, &fcmd_fclose, &fcmd_open, &fcmd_edit, &acmd_list, 
		&fcmd_jump, &acmd_quit, &acmd_fquit, NULL
	};

	cs->htbl = malloc(sizeof(struct hsearch_data));
	bzero(cs->htbl, sizeof(struct hsearch_data));
	hcreate_r(64, cs->htbl);

	// Insert commands into the hash table, accessible by both short or long name.
	for (int i = 0; CMDS[i]; ++i) {
		c = CMDS[i];
		e.data = (void *)c;
		e.key = c->short_name;
		hsearch_r(e, ENTER, &ep, cs->htbl);
		e.key = c->long_name;
		hsearch_r(e, ENTER, &ep, cs->htbl);
	}
}

