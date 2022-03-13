/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Echo line buffer. The buffer where the user enters and runs commands
 * and has status/error messages echoed back.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef ELBUF_H
#define ELBUF_H

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include "../ds/str.h"
#include "../cursor.h"
#include "fbuf.h"
#include "fbaux.h"

typedef fbuf_t elbuf_t;

/*
 * elbuf_init - Initialise a new echo line buffer
 *
 * Fields of the buffer are dynamically allocated, so free this memory
 * with elbuf_free.
 */
void elbuf_init(elbuf_t *e, WINDOW *w);

/*
 * elbuf_free - Free an echo line buffer initialised with elbuf_init
 */
void elbuf_free(elbuf_t *e);

/*
 * elbuf_line - Get an echo line buffer's only line
 */
line_t *elbuf_line(elbuf_t *e);

/*
 * elbuf_str - Get the string underlying the echo line buffer
 */
char *elbuf_str(elbuf_t *e);

/*
 * elbuf_strlen - Get the length of the string underlying the echo line buffer
 */
int elbuf_strlen(elbuf_t *e);

/*
 * elbuf_set - Set the contents of an echo line buf to a string
 * @s: null-terminated string
 */
void elbuf_set(elbuf_t *e, char *s);

#endif
