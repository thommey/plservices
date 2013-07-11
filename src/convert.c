/**
 * convert.c - Argument conversion (string to time_t, struct chan/user*)
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
#include <limits.h>
#include <string.h>

#include "main.h"

void convert_nick(struct funcarg *a, char *str) {
	a->data.p = get_user_by_nick(str);
	a->type = a->data.p ? ARGTYPE_PTR : ARGTYPE_NONE;
}

void convert_unum(struct funcarg *a,char *str) {
	a->data.p = get_user_by_numeric(str);
	a->type = a->data.p ? ARGTYPE_PTR : ARGTYPE_NONE;
}

void convert_snum(struct funcarg *a,char *str) {
	a->data.p = get_server_by_numeric(str);
	a->type = a->data.p ? ARGTYPE_PTR : ARGTYPE_NONE;
}

void convert_num(struct funcarg *a, char *str) {
	if (strlen(str) == SNUMLEN)
		a->data.p = get_server_by_numeric(str);
	else if (strlen(str) == UNUMLEN)
		a->data.p = get_user_by_numeric(str);
	else
		a->data.p = NULL;
	a->type = a->data.p ? ARGTYPE_PTR : ARGTYPE_NONE;
}

void convert_chan(struct funcarg *a, char *str) {
	static struct channel chan0 = { MAGIC_CHANNEL0 };
	a->data.p = !strcmp(str, "0") ? &chan0 : get_channel_by_name(str);
	a->type = a->data.p ? ARGTYPE_PTR : ARGTYPE_NONE;
}

void convert_uint(struct funcarg *a, char *str) {
	char *endptr;

	a->type = ARGTYPE_NONE;

	if (!str[0])
		return;

	a->data.u = (unsigned int)strtoul(str, &endptr, 10);

	if (*endptr != '\0')
		return;

	a->type = ARGTYPE_UINT;
}

void convert_long(struct funcarg *a, char *str) {
	char *endptr;

	a->type = ARGTYPE_NONE;

	if (!str[0])
		return;

	a->data.l = strtol(str, &endptr, 10);

	if (*endptr != '\0')
		return;

	a->type = ARGTYPE_LONG;
}

void convert_time(struct funcarg *a, char *str) {
	char *endptr;

	a->type = ARGTYPE_NONE;

	if (!str[0])
		return;

	a->data.t = strtoull(str, &endptr, 10);

	if (*endptr != '\0')
		return;

	a->type = ARGTYPE_TIME;
}
