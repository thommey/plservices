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

#ifndef UTILS_H_
#define UTILS_H_

#include <time.h>
#include <stddef.h>

#define strbufcpy(d, s) strncpyz((d), (s), sizeof(d))

char * strtolower (char *str);
char * strtoupper (char *str);
char *sstrdup(const char *str);
void sfree(void *p);
void *smalloc(size_t s);
void *zmalloc(size_t s);
char *strncpyz(char *dest, const char *src, size_t n);
long *randomset(int count, long max);

#endif /* UTILS_H_ */
