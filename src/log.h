/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Diagnostic and error logging.
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#ifndef LOG_H
#define LOG_H

/*
 * Log a line to file ~/.tedit-log/<pid> using printf style formatting where 
 * <pid> is the process ID of the running process.
 * 
 * The line is of format "[<timestamp>]: <input>" where <timestamp> is the 
 * current datetime, <input> is the input parameters after formatting.
 */
void tlog(char *fmt, ...);

#endif
