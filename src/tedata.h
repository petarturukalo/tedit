/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Main text editor data used to run the program.
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef TEDATA_H
#define TEDATA_H

#include <search.h>
#include <curses.h>
#include <semaphore.h>
#include "fbuf/elbuf.h"
#include "fbuf/bufs.h"
#include "cmd/cmd.h"
#include "synhl/clrmap.h"

struct text_editor_data {
	WINDOW *win;  // Curses window for displaying file buffers in.
	bufs_t bufs;
	cmds_t cmds;
	clrmap_t clrmap;
	// Lock which must be acquired before accessing other fields
	// of this data structure.
	sem_t sem;  
};

typedef struct text_editor_data tedata_t;


/*
 * tedata_init - Set default values for text editor data fields.
 * @fpaths: paths to files to initially open. This array is NULL pointer terminated.
 *
 * Meant to be called before having started any threads that use the data.
 * Return whether initialisation was successful.
 */
bool tedata_init(tedata_t *t, char *fpaths[]);

/*
 * tedata_free - Clean up the text editor data
 *
 * Doesn't use any explicit mallocs (called free for consistency).
 */
void tedata_free(tedata_t *t);

/*
 * setup_curses - Set up default curses settings
 */
void setup_curses(void);

#endif
