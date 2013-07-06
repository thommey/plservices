/**
 * convert.c - Argument conversion (string to time_t, struct chan/user*)
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
**/

#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "main.h"

#define CACHESIZE 32

static unsigned int uintcache[CACHESIZE];
static long longcache[CACHESIZE];
static time_t timecache[CACHESIZE];

static size_t uintcacheidx;
static size_t longcacheidx;
static size_t timecacheidx;

struct user *convert_nick(char *str) {
	if (!str || !str[0])
		return NULL;

	return get_user_by_nick(str);
}

struct user *convert_unum(char *str) {
	if (strlen(str) != UNUMLEN)
		return NULL;

	return get_user_by_numeric(str);
}

struct server *convert_snum(char *str) {
	if (strlen(str) != SNUMLEN)
		return NULL;

	return get_server_by_numeric(str);
}

struct entity *convert_num(char *str) {
	if (!str || !str[0])
		return NULL;

	if (strlen(str) == SNUMLEN)
		return (struct entity *)convert_snum(str);
	if (strlen(str) == UNUMLEN)
		return (struct entity *)convert_unum(str);

	return NULL;
}

struct channel *convert_chan(char *str) {
	static struct channel chan0 = { MAGIC_CHANNEL0 };

	if (!strcmp(str, "0"))
		return &chan0;

	return get_channel_by_name(str);
}

unsigned int *convert_uint(char *str) {
	char *endptr;

	if (!str[0])
		return NULL;

	if (uintcacheidx >= CACHESIZE)
		error("uintcache exhausted");

	uintcache[uintcacheidx] = (int)strtol(str, &endptr, 10);

	if (*endptr != '\0')
		return NULL;

	return &uintcache[uintcacheidx++];
}

long *convert_long(char *str) {
	char *endptr;

	if (!str[0])
		return NULL;

	if (longcacheidx >= CACHESIZE)
		error("longcache exhausted");

	longcache[longcacheidx] = strtol(str, &endptr, 10);

	if (*endptr != '\0')
		return NULL;

	return &longcache[longcacheidx++];
}

time_t *convert_time(char *str) {
	char *endptr;

	if (!str[0])
		return NULL;

	if (timecacheidx >= CACHESIZE)
		error("timecache exhausted");

	timecache[timecacheidx] = (time_t)strtoll(str, &endptr, 10);

	if (*endptr != '\0')
		return NULL;

	return &timecache[timecacheidx++];
}

void free_conversion(void) {
	uintcacheidx = 0;
	longcacheidx = 0;
	timecacheidx = 0;
}
