/**
 * utils.c - Various utility functions, memory management
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

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "utils.h"
#include "log.h"

void sfree(void *p) {
	if (!p) {
		logtxt(LOG_WARNING, "Attempted to free null pointer");
		return;
	}
	free(p);
}

void *smalloc(size_t s) {
	void *p = malloc(s);
	if (!p) {
		logtxt(LOG_WARNING, "Out of memory");
	}
	return p;
}

void *zmalloc(size_t s) {
	void *p = smalloc(s);
	if (p)
		memset(p, 0, s);
	return p;
}

char *strncpyz(char *dest, const char *src, size_t n) {
	strncpy(dest, src, n);
	if (n > 0)
		dest[n-1] = '\0';
	return dest;
}

void shift(struct args *arg, int offset) {
	arg->c += offset;
	if (offset > 0)
		memmove(arg->v + offset, arg->v, arg->c * sizeof(*arg->v));
	else if (offset < 0) {
		offset = -offset;
		memmove(arg->v, arg->v + offset, arg->c * sizeof(*arg->v));
	}
}

#undef pack
struct args pack(enum argtype current, ...) {
	struct args arg;
	va_list ap;

	arg.c = 0;
	if (current == ARGTYPE_NONE)
		return arg;

	va_start(ap, current);
	while (current != ARGTYPE_NONE) {
		switch (current) {
			case ARGTYPE_INT:
				arg.v[arg.c].data.i = va_arg(ap, int);
				break;
			case ARGTYPE_UINT:
				arg.v[arg.c].data.u = va_arg(ap, unsigned int);
				break;
			case ARGTYPE_LONG:
				arg.v[arg.c].data.l = va_arg(ap, long);
				break;
			case ARGTYPE_ULONG:
				arg.v[arg.c].data.ul = va_arg(ap, unsigned long);
				break;
			case ARGTYPE_TIME:
				arg.v[arg.c].data.t = va_arg(ap, time_t);
				break;
			case ARGTYPE_PTR:
				arg.v[arg.c].data.p = va_arg(ap, void *);
				break;
			default:
				logfmt(LOG_WARNING, "Unknown arg type: %d", current);
				return arg;
		}
		arg.v[arg.c++].type = current;
		current = va_arg(ap, enum argtype);
	}
	return arg;
};

int argdata_int(struct funcarg *a) {
	assert(a->type == ARGTYPE_INT);
	return a->data.i;
}

unsigned int argdata_uint(struct funcarg *a) {
	assert(a->type == ARGTYPE_UINT);
	return a->data.u;
}

long argdata_long(struct funcarg *a) {
	assert(a->type == ARGTYPE_LONG);
	return a->data.l;
}

time_t argdata_time(struct funcarg *a) {
	assert(a->type == ARGTYPE_TIME);
	return a->data.t;
}

void *argdata_ptr(struct funcarg *a) {
	assert(a->type == ARGTYPE_PTR);
	return a->data.p;
}


/* generates count random numbers from 0 to max-1 */
/* Knuth's algorithm */
long *randomset(int count, long max) {
	static long *randnums = NULL;
	static int randnumssize = 0;
	int in, im, rn, rm;

	/* allocate array of numbers that has enough room */
	if (randnums == NULL) {
		srand(time(NULL));
		randnums = malloc(sizeof(*randnums) * count * 2);
		randnumssize = count * 2;
	} else if (randnumssize < count) {
		randnums = realloc(randnums, sizeof(*randnums) * count * 2);
		randnumssize = count * 2;
	}
	for (im = 0, in = 0; in < max && im < count; in++) {
		rn = max - in;
		rm = count - im;
		if (rand() % rn < rm)
			randnums[im++] = in;
	}
	return randnums;
}
