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
**/

#include <stdlib.h>
#include <string.h>

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
