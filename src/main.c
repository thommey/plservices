/**
 * main.c
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

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "main.h"

time_t now;

void debug_print_channels(void);
void debug_print_users(void);

int main(int argc, char **argv) {
	time_t last;

	if (argc != 6) {
		fprintf(stderr, "Syntax: %s <IP> <PORT> <PASS> <NAME (ex services.whatever.org)> <DESCR (here are my services)>\n", argv[0]);
		return 1;
	}

	init_parse();
	init_tokens();
	init_modes();

	now = time(NULL);
	net_connect(argv[1], argv[2], argv[3], argv[4], argv[5]);

	last = now = time(NULL);
	while (1) {
		if (last != now) {
			lua_ghook("ontick", NULL);
			lua_ghook("ontick2", NULL); // TODO scheduler in C?
			last = now;
		}
//		debug_print_channels();
//		debug_print_users();
		net_read();
		now = time(NULL);
	}
	return 0;
}
