/**
 * log.c - Logging
 *
 * Copyright (c) 2013 Thomas Sader (thommey)
 *
 *  This file is part of PLservices.
 *
 *  PLservices is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PLservices is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PLservices.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */

#include <stdarg.h>
#include <stdio.h>

#include "main.h"

static int debuglvl = LOG_DEBUG|LOG_ERROR|LOG_FATAL|LOG_WARNING|LOG_LUA|LOG_RAW;

#undef logfmt
#undef logtxt

void logfmtva(int loglevel, const char *fmt, va_list ap) {
	if (!(debuglvl & loglevel))
		return;
	vfprintf(stderr, fmt, ap);
}

void logfmt(int loglevel, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	logfmtva(loglevel, fmt, ap);
	va_end(ap);
}

void logtxt(int loglevel, const char *text) {
	if (!(debuglvl & loglevel))
		return;
	fprintf(stderr, text);
}
