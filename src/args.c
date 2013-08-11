/**
 * args.c - Handling and packing of multiple words into argument structs
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

#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "args.h"


/* shifts args by offset, can be negative */
void shift(struct args *arg, int offset) {
	arg->c += offset;
	if (offset > 0)
		memmove(arg->v + offset, arg->v, arg->c * sizeof(*arg->v));
	else if (offset < 0) {
		offset = -offset;
		memmove(arg->v, arg->v + offset, arg->c * sizeof(*arg->v));
	}
}

/* argument packing */
struct funcarg arg_int(int i) {
	return (struct funcarg) {
		.type = ARGTYPE_INT, .data.i = i
	};
}

struct funcarg arg_uint(unsigned int u) {
	return (struct funcarg) {
		.type = ARGTYPE_UINT, .data.u = u
	};
}

struct funcarg arg_long(long l) {
	return (struct funcarg) {
		.type = ARGTYPE_LONG, .data.l = l
	};
}

struct funcarg arg_ulong(unsigned long ul) {
	return (struct funcarg) {
		.type = ARGTYPE_ULONG, .data.ul = ul
	};
}

struct funcarg arg_time(time_t t) {
	return (struct funcarg) {
		.type = ARGTYPE_TIME, .data.t = t
	};
}

struct funcarg arg_user(struct user *u) {
	return (struct funcarg) {
		.type = ARGTYPE_USER, .data.p = u
	};
}

struct funcarg arg_chan(struct channel *c) {
	return (struct funcarg) {
		.type = ARGTYPE_CHAN, .data.p = c
	};
}

struct funcarg arg_server(struct server *s) {
	return (struct funcarg) {
		.type = ARGTYPE_SERVER, .data.p = s
	};
}

struct funcarg arg_str(char *s) {
	return (struct funcarg) {
		.type = ARGTYPE_STR, .data.p = s
	};
}

struct funcarg arg_ptr(void *p) {
	return (struct funcarg) {
		.type = ARGTYPE_PTR, .data.p = p
	};
}

#undef pack_args
struct args pack_args(struct funcarg first, ...) {
	struct args arg;
	struct funcarg current;
	va_list ap;

	va_start(ap, first);
	arg.c = 0;
	current = first;
	while (current.type != ARGTYPE_NONE) {
		arg.v[arg.c++] = current;
		current = va_arg(ap, struct funcarg);
	}
	return arg;
}

#undef pack_words
struct args pack_words(const char *first, ...) {
	struct args arg;
	const char *current = first;
	va_list ap;

	va_start(ap, first);
	arg.c = 0;
	while (current) {
		arg.v[arg.c].type = ARGTYPE_STR;
		arg.v[arg.c].data.p = (char *)current;
		current = va_arg(ap, const char *);
		arg.c++;
	}
	return arg;
}

struct args pack_empty(void) {
	return (struct args) {
		.c = 0
	};
}

/* argument unpacking */
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

struct user *argdata_user(struct funcarg *a) {
	assert(a->type == ARGTYPE_USER);
	return a->data.p;
}

struct channel *argdata_chan(struct funcarg *a) {
	assert(a->type == ARGTYPE_CHAN);
	return a->data.p;
}

struct server *argdata_server(struct funcarg *a) {
	assert(a->type == ARGTYPE_SERVER);
	return a->data.p;
}

char *argdata_str(struct funcarg *a) {
	assert(a->type == ARGTYPE_STR);
	return a->data.p;
}

void *argdata_ptr(struct funcarg *a) {
	assert(a->type == ARGTYPE_PTR);
	return a->data.p;
}
