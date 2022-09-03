/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#include "cmd.h"

void cmds_free(cmds_t *c)
{
	hdestroy_r(c->htbl);
	free(c->htbl);
}

/*
 * The current minimum and maximum lengths of a short name in the cmd_t struct. 
 * The max is subject to change if a command with a longer short name is added.
 */
#define MIN_SHORT_NAME_LEN 1
#define MAX_SHORT_NAME_LEN 2

/*
 * Look up a command by name using an approximate search. This allows the user to
 * enter a command name inbetween its short name and long name and have it resolve
 * to the short name.
 *
 * For example, the command close has short name "c" and long name "close". An approximate
 * search has the user type a string matching regex pattern c.* resolve to the close command: 
 * the user could type "cl", "cls", or even "cjjj", etc.
 *
 * Because some commands have short names with 2 characters, prefixes are searched for in descending 
 * order of length: first the prefix of length 2 is checked, then the prefix of length 1. The "fclose"
 * command for instance has prefix "fc" of length 2. If a new command "foo" was added in the future with
 * short name "f", if prefixes weren't searched for in descending order of length, then entering "fclo" would
 * first check for a prefix of length 1 "f", and resolve to command "foo", when it should have instead
 * resolved to "fclose". Searching in descending order of prefix length fixes this.
 */
cmd_t *cmds_search_approx(cmds_t *c, char *name)
{
	if (strlen(name) > MIN_SHORT_NAME_LEN) {
		ENTRY e, *ep;
		char name_prefix[MAX_SHORT_NAME_LEN+1];
		int prefix_len;

		name_prefix[0] = '\0';
		strncat(name_prefix, name, MAX_SHORT_NAME_LEN);
		e.key = name_prefix;

		prefix_len = strlen(name_prefix);
		for (; prefix_len > 0; --prefix_len) {
			name_prefix[prefix_len] = '\0';
			if (hsearch_r(e, FIND, &ep, c->htbl))
				return (cmd_t *)ep->data;
		}
	}
	return NULL;
}

cmd_t *cmds_search(cmds_t *c, char *name)
{
	ENTRY e, *ep;

	e.key = name;
	if (hsearch_r(e, FIND, &ep, c->htbl))
		return (cmd_t *)ep->data;
	return cmds_search_approx(c, name);
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

