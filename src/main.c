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
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "main.h"

extern char *optarg;

time_t now;

void debug_print_channels(void);
void debug_print_users(void);

static void init(void) {
	init_hooks();
	init_base64();
	init_stringutils();
	init_tokens();
	init_modes();
//	init_burster();
	hook_hook("onregistered", module_loadAll);
}

int main(int argc, char **argv) {
	char configfile[64] = "./plservices.conf";
	time_t last;
	int opt;
	system_argv = argv[0];

	while ((opt = getopt(argc, argv, "c:")) != -1) {
		switch (opt) {
			case 'c':
				strbufcpy(configfile, optarg);
				break;
			case '?':
				fprintf(stderr, "Usage: %s [-c config]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	init();
	load_config(configfile);
	now = time(NULL);
	net_connect();

	last = now = time(NULL);
	while (1) {
		now = time(NULL);
		if (last != now) {
			hook_call("ontick", pack_empty());
		}
		net_read();
		last = now;
	}
	return 0;
}
