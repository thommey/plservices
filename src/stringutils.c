/**
 * string.c - String related functions
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

#define BUFLEN 512

#include <stddef.h>

static char rfc_tolower_table[256];
static char *rfc_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ[])\\";
static char *rfc_lower = "abcdefghijklmnopqrstuvwxyz{}|";

void init_stringutils(void) {
	int i;

	for (i = 0; i < sizeof(rfc_tolower_table); i++)
		rfc_tolower_table[i] = i;
	for (i = 0; rfc_upper[i]; i++)
		rfc_tolower_table[(unsigned char)rfc_upper[i]] = rfc_lower[i];
}

char *strconv(int (*convert)(int), char *buf, size_t buflen, const char *str) {
	char mybuf[BUFLEN+1];
	int i;

	if (!buf) {
		buf = mybuf;
		buflen = sizeof(mybuf);
	}
    for (i = 0; i < buflen - 1 && str[i]; i++)
		buf[i] = convert((unsigned char)str[i]);
	buf[i] = '\0';
	return buf;
}

int rfc_tolower(int in) {
	return rfc_tolower_table[(unsigned char)in];
}
