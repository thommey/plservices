/**
 * tokeninfo.c - Table of P10 tokens and their parsing rules
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
#include <assert.h>

#include "jtable.h"

static jtableS tokeninfos;
static struct tokeninfo *shorttokeninfos[256];

struct tokeninfo *get_tokeninfo(char *token) {
	assert(token && token[0]);
	if (!token[1])
		return shorttokeninfos[(unsigned char)token[0]];
	else
		return jtableS_get(&tokeninfos, token);
};

struct tokeninfo *add_tokeninfo(char *token, struct tokeninfo *info) {
	assert(token && token[0]);
	if (!token[1])
		shorttokeninfos[(unsigned char)token[0]] = info;
	else
		jtableS_insert(&tokeninfos, token, info);
	return info;
};
