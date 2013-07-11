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
 */

#include <string.h>
#include <stdio.h>

#include "main.h"

static char rfc_tolower_table[256];
static char *rfc_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ[])\\";
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
		strncpy(pos, argv[i], bufsize-(buf-pos)-3);
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
			ret.v[ret.c].type = ARGTYPE_PTR;
			ret.v[ret.c].data.p = NULL;
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
			(rule->r[ret.c].convert)(&ret.v[ret.c], argv[argsrc]);
			if (ret.v[ret.c].type == ARGTYPE_NONE) {
				if (!rule_optional(rule->r[ret.c]))
					return NULL;
				/* conversion failed */
				continue;
			}
		} else {
			ret.v[ret.c].type = ARGTYPE_PTR;
			ret.v[ret.c].data.p = argv[argsrc];
		}
		usedarg[argsrc] = 1;
		consumed++;
	}
	/* postponed greedy arg consuming */
	if (greedyarg != -1) {
		for (greed.c = 0; greedyargsrc+greed.c < argc && !usedarg[greedyargsrc+greed.c]; consumed++, greed.c++)
			greed.v[greed.c] = argv[greedyargsrc+greed.c];
		ret.v[greedyarg].type = ARGTYPE_PTR;
		ret.v[greedyarg].data.p = &greed;
	}
	/* sanity check to detect protocol mismatches, not all arguments used? */
	if (consumed != argc)
		return NULL;

	return &ret;
};

/* yuck */
void call_varargs(void (*f)(), struct args *arg) {
	struct funcarg *v = arg->v;
	switch (arg->c) {
		case 0:
			f();
			break;
		case 1:
			f(argdata(v[0]));
			break;
		case 2:
			f(argdata(v[0]), argdata(v[1]));
			break;
		case 3:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]));
			break;
		case 4:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]));
			break;
		case 5:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]));
			break;
		case 6:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]), argdata(v[5]));
			break;
		case 7:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]), argdata(v[5]),
				argdata(v[6]));
			break;
		case 8:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]), argdata(v[5]),
				argdata(v[6]), argdata(v[7]));
			break;
		case 9:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]), argdata(v[5]),
				argdata(v[6]), argdata(v[7]), argdata(v[8]));
			break;
		case 10:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]), argdata(v[5]),
				argdata(v[6]), argdata(v[7]), argdata(v[8]), argdata(v[9]));
			break;
		case 11:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]), argdata(v[5]),
				argdata(v[6]), argdata(v[7]), argdata(v[8]), argdata(v[9]), argdata(v[10]));
			break;
		case 12:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]), argdata(v[5]),
				argdata(v[6]), argdata(v[7]), argdata(v[8]), argdata(v[9]), argdata(v[10]), argdata(v[11]));
			break;
		case 13:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]), argdata(v[5]),
				argdata(v[6]), argdata(v[7]), argdata(v[8]), argdata(v[9]), argdata(v[10]), argdata(v[11]),
				argdata(v[12]));
			break;
		case 14:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]), argdata(v[5]),
				argdata(v[6]), argdata(v[7]), argdata(v[8]), argdata(v[9]), argdata(v[10]), argdata(v[11]),
				argdata(v[12]), argdata(v[13]));
			break;
		case 15:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]), argdata(v[5]),
				argdata(v[6]), argdata(v[7]), argdata(v[8]), argdata(v[9]), argdata(v[10]), argdata(v[11]),
				argdata(v[12]), argdata(v[13]), argdata(v[14]));
			break;
		case 16:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]), argdata(v[5]),
				argdata(v[6]), argdata(v[7]), argdata(v[8]), argdata(v[9]), argdata(v[10]), argdata(v[11]),
				argdata(v[12]), argdata(v[13]), argdata(v[14]), argdata(v[15]));
			break;
		case 17:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]), argdata(v[5]),
				argdata(v[6]), argdata(v[7]), argdata(v[8]), argdata(v[9]), argdata(v[10]), argdata(v[11]),
				argdata(v[12]), argdata(v[13]), argdata(v[14]), argdata(v[15]), argdata(v[16]));
			break;
		case 18:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]), argdata(v[5]),
				argdata(v[6]), argdata(v[7]), argdata(v[8]), argdata(v[9]), argdata(v[10]), argdata(v[11]),
				argdata(v[12]), argdata(v[13]), argdata(v[14]), argdata(v[15]), argdata(v[16]), argdata(v[17]));
			break;
		case 19:
			f(argdata(v[0]), argdata(v[1]), argdata(v[2]), argdata(v[3]), argdata(v[4]), argdata(v[5]),
				argdata(v[6]), argdata(v[7]), argdata(v[8]), argdata(v[9]), argdata(v[10]), argdata(v[11]),
				argdata(v[12]), argdata(v[13]), argdata(v[14]), argdata(v[15]), argdata(v[16]), argdata(v[17]),
				argdata(v[18]));
			break;
		default:
			logtxt(LOG_ERROR, "Too many arguments in varargs call");
			break;
	}
}
