/**
 * burster.c - Simulate a lot of clients, channels, etc.
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

#include "main.h"

#define FAKESNUM "FB"
#define NUMCLIENTS 50000
#define NUMCHANS 40000

extern struct server *me;
extern time_t now;

static char num[6] = FAKESNUM "AAA";
static time_t mylink;
static long maxnum;

static char *nextnum(void) {
	static int first = 1;
	if (first) {
		first = 0;
		maxnum = 1;
		return num;
	}
	if (base64_incr(num + 2, 4)) {
		logtxt(LOG_ERROR, "Numerics exhausted");
		return NULL;
	}
	maxnum++;
	return num;
}

void burster_burst_clients(int max) {
	int i;
	for (i = 0; i < max; i++)
		send_format("%s N fake%ld 2 %ld f%ld f%ld.fake.com +id DAqAAB %s :fake user number %ld", FAKESNUM, i, now, i, i, nextnum(), i);
}

/* values takes from freenode */
static double data_freenode[21] = { 0, 0, 0, 2880, 1850, 1260, 1000, 870, 610, 517, 420, 360, 280, 280, 200, 200, 170, 160, 140, 140, 110 };
/* values taken from quakenet */
static double data_quakenet[21] = { 0, 10000, 5660, 3660, 2000, 1200, 870, 640, 500, 330, 300, 250, 200, 140, 120, 120, 100, 90, 80, 80, 60 };

static void burst_chanusers(int size, char *name) {
	int j;
	long *rands;
	char buf[513], *t, *c;

	rands = randomset(size, maxnum);
	t = buf + sprintf(buf, "%s B %s %ld ", FAKESNUM, name, mylink);
	for (c = t, j = 0; j < size; j++) {
		if (c != t)
			*c++ = ',';
		*c++ = FAKESNUM[0];
		*c++ = FAKESNUM[1];
		base64_encode_padded(rands[j], c, 4);
		c += 3;
		if (c >= buf + sizeof(buf) - 10 || j == size - 1) {
			send_format("%.*s", c-buf, buf);
			c = t;
		}
	}

}

static int burst_channels(int size, int count) {
	char name[200];
	int i;

	for (i = 0; i < count; i++) {
		sprintf(name, "#fake_%d_%d", size, i);
		burst_chanusers(size, name);
	}
	return count;
}

static int size_1000(int size, int total) {
	if (size < 1000)
		return 0;
	else
		return 10;
}

static int chancount_freenode(int size, int total) {
	int chancount;
	if (size < sizeof(data_freenode)/sizeof(*data_freenode))
		chancount = (total*data_freenode[size]/14000);
	else
		chancount = rand()%2 ? rand()%25 : 0;
	return chancount;
}

static int chancount_quakenet(int size, int total) {
	int chancount;
	if (size < sizeof(data_quakenet)/sizeof(*data_quakenet))
		chancount = (int)((total*data_quakenet[size]/(double)29000));
	else
		chancount = rand()%2 ? rand()%25 : 0;
	return chancount;
}

void burster_burst_channels(int max, int (*count_by_size)(int size, int total)) {
	int size, i;

	for (i = 0, size = 1; i < max; size++) {
		i += burst_channels(size, count_by_size(size, max));
	}
}

void burster_go(void) {
	mylink = now;
	send_format("%s S burster.metairc.net 2 0 %ld J10 %s]]] +sn :Mass fake server", ME, mylink, FAKESNUM);
	burster_burst_clients(100000);
	burster_burst_channels(60000, chancount_quakenet);
	send_format("%s EB", FAKESNUM);
	send_format("%s EA", FAKESNUM);
}

void init_burster(void) {
	hook_hook("onregistered", burster_go);
	return;
}

