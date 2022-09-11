/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2022 Petar Turukalo
 */
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>
#include "log.h"
#include "misc.h"

/*
 * Open the running proc's log file.
 * @dirname: directory log file is in
 */
static int log_open(const char *dirname)
{
	char fpath[128];
	snprintf(fpath, sizeof(fpath), "%s/%d", dirname, getpid());
	return open(fpath, O_CREAT|O_WRONLY|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
}

/*
 * Log a timestamp to a file, e.g. "[Sat 10 Sep 2022, 11:48:07]: " 
 * (the time is in 24 hour time).
 */
static void log_timestamp(int fd)
{
	static char tbuf[64];
	time_t cur_time = time(NULL);
	struct tm *time = localtime(&cur_time);
	size_t sz = strftime(tbuf, sizeof(tbuf), "[%a %d %b %Y, %T]: ", time);

	if (sz)
		write(fd, tbuf, sz);
}

/* Log a single newline to a file. */
static void log_newline(int fd)
{
	char nl = '\n';
	write(fd, &nl, 1);
}

/*
 * Store the log directory "~/.tedit_log" in out_logdir.
 * @sz: size of out_logdir
 */
static void getlogdir(char *out_logdir, size_t sz)
{
	if (!out_logdir[0]) {
		char *home = getenv("HOME");
		if (home)
			snprintf(out_logdir, sz, "%s/.tedit_log", home);
	}
}

void tlog(char *fmt, ...)
{
	static char logdir[64] = { 0 };
	static int fd = -1;

	if (fd == -1) {
		getlogdir(logdir, sizeof(logdir));
		if (!logdir[0])
			return;
		// Create log directory if it doesn't already exist.
		if (!file_exists(logdir)) {
			if (mkdir(logdir, 0777) == -1) {
				return;
			}
		}
		// Create log file.
		fd = log_open(logdir);
		flock(fd, LOCK_EX);
	} 
	if (fd != -1 ) {
		// Log line to file.
		va_list ap;

		log_timestamp(fd);
		va_start(ap, fmt);
		vdprintf(fd, fmt, ap);
		va_end(ap);
		log_newline(fd);
	}
}
