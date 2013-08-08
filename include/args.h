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

#ifndef ARGS_H_
#include <time.h>

/* maximum number of real arguments in a command (after parsing) */
#define MAXARGS 20

/* structures to hold an array of char* with their count in c,
 * v[i] might be misused to hold a pointer to such a struct itself
 * in case of a greedy argument */
struct manyargs {
	unsigned char c;
	char *v[256];
};

enum argtype { ARGTYPE_NONE, ARGTYPE_INT, ARGTYPE_UINT, ARGTYPE_LONG, ARGTYPE_ULONG, ARGTYPE_TIME, ARGTYPE_STR, ARGTYPE_PTR };

union argdata {
	int i;
	unsigned int u;
	long l;
	unsigned long ul;
	time_t t;
	char *s;
	void *p;
};

struct funcarg {
	enum argtype type;
	union argdata data;
};

struct args {
	unsigned char c;
	struct funcarg v[MAXARGS];
};

int argdata_int(struct funcarg *a);
unsigned int argdata_uint(struct funcarg *a);
long argdata_long(struct funcarg *a);
unsigned long argdata_ulong(struct funcarg *a);
time_t argdata_time(struct funcarg *a);
char *argdata_str(struct funcarg *a);
void *argdata_ptr(struct funcarg *a);
void shift(struct args *arg, int amount);

struct args pack_args(struct funcarg first, ...);
struct args pack_words(const char *first, ...);
struct args pack_empty(void);
struct funcarg arg_int(int i);
struct funcarg arg_uint(unsigned int u);
struct funcarg arg_long(long l);
struct funcarg arg_ulong(unsigned long ul);
struct funcarg arg_time(time_t t);
struct funcarg arg_str(char *s);
struct funcarg arg_ptr(void *p);

#define pack_words(...) pack_words(__VA_ARGS__, NULL)
#define pack_args(...) pack_args(__VA_ARGS__, (struct funcarg) {.type = ARGTYPE_NONE})
#define argdata(arg) ((arg).type == ARGTYPE_INT  ? &(arg).data.i : \
					  (arg).type == ARGTYPE_UINT ? &(arg).data.u : \
					  (arg).type == ARGTYPE_LONG ? &(arg).data.l : \
					  (arg).type == ARGTYPE_ULONG ? &(arg).data.ul : \
					  (arg).type == ARGTYPE_TIME ? &(arg).data.t : \
					  (arg).type == ARGTYPE_STR ? (arg).data.s : \
					  (arg).type == ARGTYPE_PTR  ? (arg).data.p : NULL)

#endif // ARGS_H_
