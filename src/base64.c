/**
 * base64.c - P10's version of base64
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

#include "log.h"
#include "base64.h"

static char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789[]";
static char base64table[256];

#define encode_base64_digit(d) base64chars[(unsigned char)(d)]
#define decode_base64_digit(c) base64table[(unsigned char)(c)]

void init_base64(void) {
	int i;
	for (i = 0; i < sizeof(base64table); i++)
		decode_base64_digit(i) = -1;
	for (i = 0; encode_base64_digit(i); i++)
		decode_base64_digit(encode_base64_digit(i)) = i;
}

/* len = 0 for reading until \0 */
long base64_decode_long(const char *str, size_t len) {
	int i = 0;
	long result = 0;

	for (i = 0; len ? i < len : str[i]; i++) {
		result *= 64;
		result += decode_base64_digit(str[i]);
	}
	return result;
}

char *base64_encode_padded(char *buf, size_t bufsize, unsigned long num) {
	char *e;

	if (bufsize < 2) {
		logtxt(LOG_ERROR, "Base64 encode with too small bufsize");
		return NULL;
	}

	e = buf + bufsize - 1;
	*e = '\0';
	do {
		*(--e) = encode_base64_digit(num % 64);
		num /= 64;
	} while (e > buf);
	return buf;
}

/* increase a base64 value, returns the number of times it wrapped around */
int base64_incr(char *buf, size_t bufsize) {
	char *e, t;

	if (bufsize < 2) {
		logtxt(LOG_ERROR, "Base64 incr with too small bufsize");
		return 1;
	}

	e = buf + bufsize - 1;
	while (--e >= buf) {
		t = decode_base64_digit(*e);
		if (t == 63)
			*e = base64chars[0];
		else {
			*e = encode_base64_digit(t + 1);
			return 0;
		}
	}
	return 1;
}

unsigned long str2unum(const char *str) {
	return base64_decode_long(str, 5);
}

unsigned long str2snum(const char *str) {
	return base64_decode_long(str, 2);
}

const char *unum2str(unsigned long unum) {
	static char buf[UNUMLEN+1];
	base64_encode_padded(buf, sizeof(buf), unum);
	return buf;
}

const char *snum2str(unsigned long snum) {
	static char buf[SNUMLEN+1];
	base64_encode_padded(buf, sizeof(buf), snum);
	return buf;
}

const char *cnum2str(unsigned long snum, unsigned long cnum) {
	static char buf[UNUMLEN+1];
	base64_encode_padded(buf, sizeof(buf), snum*64*64*64+cnum);
	return buf;
}
