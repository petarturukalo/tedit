/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#include "stack.h"

void *stack_peek(mystack_t *s)
{
	if (!s->len)
		return NULL;
	return dlist_get_address(s, s->len-1);
}
