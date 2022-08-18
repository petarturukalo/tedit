/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Stack data structure.
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#ifndef STACK_H
#define STACK_H

#include "dlist.h"

typedef dlist_t mystack_t;
#define stack_init(stack, eltsz)	dlist_init(stack, DLIST_MIN_CAP, eltsz)
#define stack_push(stack, elem)		dlist_append(stack, elem)
#define stack_pop(stack, out_elem)	dlist_pop(stack, out_elem)
/* Get the element at the top of the stack. Return NULL if the stack is empty. */
void *stack_peek(mystack_t *s);

#endif
