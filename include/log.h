/**
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

#ifndef LOG_H_
#define LOG_H_

#define LOG_FATAL      0x0001
#define LOG_ERROR      0x0010
#define LOG_WARNING    0x0100
#define LOG_RAW        0x0400
#define LOG_LUA        0x1000
#define LOG_DEBUGIO    0x2000
#define LOG_DEBUG      0x4000
#define LOG_PARSEDEBUG 0x8000

#define error(msg) do { logfmt(LOG_FATAL, "Fatal error at %s (line %d): %s\n", __FILE__, __LINE__, msg); exit(1); } while (0)

void logfmt(int loglevel, const char *fmt, ...);
void logtxt(int loglevel, const char *text);

#define logtxt(l, f) logtxt(l, f "\n")
#define logfmt(l, f, ...) logfmt(l, f "\n", __VA_ARGS__)

#define POSIXERR(str) do { logfmt(LOG_FATAL, "POSIX error: %s", strerror(errno)); error(str); } while(0)

#endif /* LOG_H_ */
