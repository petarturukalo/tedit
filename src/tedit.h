/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2021 Petar Turukalo
 */
#ifndef TEDIT_H
#define TEDIT_H

#include <pthread.h>
#include <signal.h>
#include "tedata.h"
#include "sig.h"
#include "display.h"
#include "input.h"
#include "tab.h"
#include "fbuf/fbuf.h"
#include "fbuf/fbio.h"

/**
 * tedit_start - Entry point to starting the text editor.
 * @fpaths: paths to files to open
 * @nfpaths: number of files to open
 *
 * If no filepaths are given then an empty, unnamed file buffer is used.
 */
void tedit_start(char *fpaths[], int nfpaths);

#endif
