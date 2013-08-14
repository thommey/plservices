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
#include <unistd.h>
#include <ctype.h>

#include "utils.h"
#include "log.h"

char *strtolower(char *str){
    	char *newstr, *p;
    	p = newstr = strdup(str);
    	while(*p++=tolower(*p));
	return newstr;
}


char *strtoupper(char *str){
    	char *newstr, *p;
    	p = newstr = strdup(str);
    	while(*p++=toupper(*p));
    	return newstr;
}


void sfree(void *p) {
	free(p);
}

void *smalloc(size_t s) {
	void *p = malloc(s);
	if (!p) {
		logfmt(LOG_FATAL, "Out of memory. Could not allocate %zd bytes.", s);
		exit(1);
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
