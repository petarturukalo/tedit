/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * File buffer commands. Commands which operate on file buffers
 * (writing, opening, closing, etc.).
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef FCMD_H
#define FCMD_H

#include "cmd.h"
#include "../chrp.h"

/*
 * Command for writing to a file.
 * Either write the currently active file buffer to its currently linked file on
 * disk or to a new file on disk.
 */
extern cmd_t fcmd_write;

/*
 * Command for attempting to close the currently active file buffer
 * (the file must be saved before closing).
 */
extern cmd_t fcmd_close;

/*
 * Command for force closing a file buffer (the file doesn't need to be saved).
 */
extern cmd_t fcmd_fclose;

/*
 * Command for opening a new file. 
 */
extern cmd_t fcmd_open;

/*
 * Command for editing an existing file buffer.
 */
extern cmd_t fcmd_edit;

/*
 * Command for jumping to a file buffer by id.
 * Exists so unnamed file buffers can be jumped to and edited.
 */
extern cmd_t fcmd_jump;

#endif
