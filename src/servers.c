/**
 * server.c - Server management
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

#include "main.h"

static jtableL serverlist;

struct server *me, *uplink;

struct server *get_server_by_numeric(unsigned long numeric) {
	return jtableL_get(&serverlist, numeric);
}

struct server *get_server_by_numericstr(const char *numeric) {
	return get_server_by_numeric(str2snum(numeric));
}

struct server *add_server(unsigned long numeric, char *maxusers, char *name, int hops, time_t boot, time_t link, char *protocol, char *description) {
	struct server *s;

	s = zmalloc(sizeof(*s));
	s->magic = MAGIC_SERVER;
	s->numeric = numeric;
	strbufcpy(s->maxusers, maxusers);
	strbufcpy(s->name, name);
	s->hops = hops;
	s->boot = boot;
	s->link = link;
	strbufcpy(s->protocol, protocol);
	strbufcpy(s->description, description);

	/* first one is myself, second one is my uplink */
	if (!me)
		me = s;
	else if (!uplink) {
		uplink = s;
		set_registered(1);
	}

	return jtableL_insert(&serverlist, numeric, s);
}

void del_server(struct server *server) {
	jtableP_iterate(&server->users, del_user_iter, NULL);
	jtableL_remove(&serverlist, server->numeric);
	free(server);
}

struct server *get_server(struct server *s) {
	if (!s || !verify_server(s))
		return NULL;
	return s;
};

static int server_modehook(struct entity *from, struct entity *target, int pls, char modechange, char *param) {
	struct server *s;

	s = (struct server *)target;

	if (!verify_server(s)) {
		logtxt(LOG_ERROR, "Mode change for not-a-server!");
		return MODEHOOK_ERROR;
	}

	/* only uplink modes relevant for now */
	if (s != uplink)
		return MODEHOOK_OK;

	if (modechange == 'n')
		pls ? uplink_with_opername() : uplink_without_opername();

	return MODEHOOK_OK;
}

void server_apply_mode(struct entity *from, struct entity *target, char *modechanges, struct manyargs *arg, int skip) {
	if (!verify_server(target)) {
		logtxt(LOG_WARNING, "Mode change on non-channel");
		return;
	}
	mode_apply(from, target, &((struct server *)target)->mode, modechanges, arg, skip, server_modehook);
}
