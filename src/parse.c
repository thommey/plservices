/**
 * parse.c - Various RFC1459 routines (split, tolower, join),
 *           handler calling for tokens and argument conversion/arrangement
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

#include <string.h>
#include <stdio.h>

#include "main.h"
#include "convert.h"

static char rfc_tolower_table[256];
static char *rfc_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ[]\\";
static char *rfc_lower = "abcdefghijklmnopqrstuvwxyz{}|";

void init_parse(void) {
	int i;

	for (i = 0; i < sizeof(rfc_tolower_table); i++)
		rfc_tolower_table[i] = i;
	for (i = 0; rfc_upper[i]; i++)
		rfc_tolower_table[(unsigned char)rfc_upper[i]] = rfc_lower[i];
}

char *rfc_tolower(char *buf, size_t bufsize, const char *str) {
	int i;

	for (i = 0; i < bufsize - 1 && str[i]; i++)
		buf[i] = rfc_tolower_table[(unsigned char)str[i]];
	buf[i] = '\0';
	return buf;
}

/* splits something (P10/RFC1459) into individual arguments.
 * ignores a leading ':', ':' at wordbeginning introduces last argument
 * otherwise whitespace splitting, stepping over multiple whitespaces
 * writes result into arg, arg->c *must* be initialized to the first free number
 */
struct manyargs *rfc_split(struct manyargs *arg, char *line) {
	arg->c = 0;

	if (!line || !line[0])
		return arg;

	if (line[0] == ':')
		line++;

	arg->v[arg->c++] = line;
	while ((line = strpbrk(line, " \r\n"))) {
		while (*line == ' ' || *line == '\r' || *line == '\n')
			*line++ = '\0';
		if (*line == ':' || !*line) {
			line++;
			break;
		}
		arg->v[arg->c++] = line;
	}
	if (line && line[0]) {
		arg->v[arg->c++] = line;
		if ((line = strpbrk(line, "\r\n")))
			*line = '\0';
	}

	return arg;
}

char *rfc_join(char *buf, size_t bufsize, int argc, char **argv, int forcecolon) {
	char *pos;
	int i;
	size_t len;

	for (pos = buf, i = 0; i < argc; i++) {
		if (i)
			*pos++ = ' ';
		if (i == argc - 1 && (forcecolon || strpbrk(argv[i], " ")))
			*pos++ = ':';
		len = strlen(argv[i]);
		strncpyz(pos, argv[i], sizeof(buf)-(buf-pos));
		pos += len;
	}
	*pos++ = '\r';
	*pos++ = '\n';
	*pos = '\0';
	return buf;
}

struct manyargs *split(struct manyargs *arg, char *line, char delim) {
	arg->c = 0;

	if (!line || !line[0])
		return arg;

	while (*line == ' ' || *line == delim)
		*line++ = '\0';

	arg->v[arg->c++] = line;
	while ((line = strchr(line, delim))) {
		while (*line == ' ' || *line == delim)
			*line++ = '\0';
		if (!*line)
			break;
		arg->v[arg->c++] = line;
	}
	if (line && line[0])
		arg->v[arg->c++] = line;

	return arg;
}

/* Takes a set of rules to parse a list of arguments into another list of args,
 * that's properly sorted, the greedy argument is handled and optional args
 * set to NULL if not present. */
struct args *arrange_args(int argc, char **argv, struct parserule *rule) {
	int argsrc, greedyargsrc, greedyarg, consumed;
	int usedarg[256] = { 0 };
	static struct args ret;
	static struct manyargs greed;

	consumed = 0;
	greedyarg = -1;

	for (ret.c = 0; ret.c < rule->c; ret.c++) {
		argsrc = rule->r[ret.c].offset;

		if (argsrc < 0)
			argsrc = argc + argsrc;
		else
			argsrc = argsrc - 1;

		/* invalid argsrc (out of bounds or already consumed), optional? */
		if (argsrc < 0 || argsrc > argc || usedarg[argsrc]) {
			if (!rule_optional(rule->r[ret.c]))
				return NULL; /* non-optional parameter not found */
			ret.v[ret.c + 1] = NULL;
			continue;
		}

		/* the greedy argument eats up everything, postpone processing */
		if (rule_greedy(rule->r[ret.c])) {
			greedyarg = ret.c;
			greedyargsrc = argsrc;
			continue;
		}

		/* conversion to perform? */
		if (rule->r[ret.c].convert) {
			ret.v[ret.c + 1] = (rule->r[ret.c].convert)(argv[argsrc]);
			if (!ret.v[ret.c + 1]) {
				if (!rule_optional(rule->r[ret.c + 1]))
					return NULL;
				/* conversion failed */
				continue;
			}
		} else {
			ret.v[ret.c + 1] = argv[argsrc];
		}
		usedarg[argsrc] = 1;
		consumed++;
	}
	/* postponed greedy arg consuming */
	if (greedyarg != -1) {
		for (greed.c = 0; greedyargsrc+greed.c < argc && !usedarg[greedyargsrc+greed.c]; consumed++, greed.c++)
			greed.v[greed.c] = argv[greedyargsrc+greed.c];
		ret.v[greedyarg + 1] = (char *)&greed;
	}
	/* sanity check to detect protocol mismatches, not all arguments used? */
	if (consumed != argc)
		return NULL;

	/* + 1 for the reserved FROM argument */
	ret.c++;
	return &ret;
};

/* ugly, but I want the functions to get proper arguments without having to extract them from something */
void call_varargs(void (*f)(), int argc, void **v) {
	#define PARAM0  ()
	#define PARAM1  (v[0])
	#define PARAM2  (v[0], v[1])
	#define PARAM3  (v[0], v[1], v[2])
	#define PARAM4  (v[0], v[1], v[2], v[3])
	#define PARAM5  (v[0], v[1], v[2], v[3], v[4])
	#define PARAM6  (v[0], v[1], v[2], v[3], v[4], v[5])
	#define PARAM7  (v[0], v[1], v[2], v[3], v[4], v[5], v[6])
	#define PARAM8  (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7])
	#define PARAM9  (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8])
	#define PARAM10 (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9])
	#define PARAM11 (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10])
	#define PARAM12 (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11])
	#define PARAM13 (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12])
	#define PARAM14 (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13])
	#define PARAM15 (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14])
	#define PARAM16 (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15])
	#define PARAM17 (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[16])
	#define PARAM18 (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[16], v[17])
	#define PARAM19 (v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7], v[8], v[9], v[10], v[11], v[12], v[13], v[14], v[15], v[16], v[17], v[18])

	#define CALLCASE(n) case n:          \
				f PARAM ## n ;   \
				break;

	/* don't look at me, seriously now! */
	switch (argc) {
		CALLCASE(0)
		CALLCASE(1)
		CALLCASE(2)
		CALLCASE(3)
		CALLCASE(4)
		CALLCASE(5)
		CALLCASE(6)
		CALLCASE(7)
		CALLCASE(8)
		CALLCASE(9)
		CALLCASE(10)
		CALLCASE(11)
		CALLCASE(12)
		CALLCASE(13)
		CALLCASE(14)
		CALLCASE(15)
		CALLCASE(16)
		CALLCASE(17)
		CALLCASE(18)
		CALLCASE(19)
		default:
			error("Callhandler has too many arguments");
	}

	free_conversion();
}
