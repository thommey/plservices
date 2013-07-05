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

#include <string.h>

#include "main.h"

static jtable userlist_num = (jtable)NULL;
static jtable userlist_nick = (jtable)NULL;
static jtable opers = (jtable)NULL;

struct user *get_user_by_numeric(char *numeric) {
	return jtableS_get(&userlist_num, numeric);
}

struct user *get_user_by_nick(const char *nick) {
	return jtableS_get(&userlist_nick, rfc_tolower(nick));
}

struct user *add_user(char *numeric, int hops, char *nick, const char *user, const char *host, const char *realname) {
	struct user *u;

	u = zmalloc(sizeof(*u));
	u->magic = MAGIC_USER;
	strbufcpy(u->numeric, numeric);
	u->hops = hops;
	strbufcpy(u->nick, nick);
	strbufcpy(u->user, user);
	strbufcpy(u->host, host);
	strbufcpy(u->realname, realname);
	u->channels = (jtable)NULL;

	jtableS_insert(&userlist_nick, rfc_tolower(nick), u);
	return jtableS_insert(&userlist_num, numeric, u);
}

void del_user(struct user *user) {
	chanusers_del_user(user);
	jtableS_remove(&userlist_nick, rfc_tolower(user->nick));
	jtableS_remove(&userlist_num, user->numeric);
	jtableS_remove(&opers, user->numeric);
	free(user);
}

void user_join0(struct user *user) {
	chanusers_del_user(user);
	jtableP_free(&user->channels);
}

void user_nickchange(struct user *u, char *newnick) {
	jtableS_remove(&userlist_nick, rfc_tolower(u->nick));
	jtableS_insert(&userlist_nick, rfc_tolower(newnick), u);
	strbufcpy(u->nick, newnick);
}

#define ADDSETBUF(name) void user_set ## name (struct user *u, char *str) { if (str) strbufcpy(u->name, str); }
#define ADDUNSETBUF(name) void user_unset ## name (struct user *u) { *u->name = '\0'; }
#define ADDBUF(name) ADDSETBUF(name) ADDUNSETBUF(name)

ADDSETBUF(account)
ADDBUF(opername)
ADDBUF(fakehost)
ADDBUF(awaymsg)

void user_unsetaccount(struct user *u) {
	u->account[0] = '0';
	u->account[1] = '\0';
}

int user_modehook(struct entity *from, struct entity *target, int pls, char modechange, char *param) {
	struct user *u;

	u = (struct user *)target;
	if (!verify_user(u)) {
		logtxt(LOG_ERROR, "Mode change for not-a-user!");
		return MODEHOOK_ERROR;
	}
	switch (modechange) {
	case 'o':
		if (pls && param)
			user_setopername(u, param);
		/* fall-through */
	case 'O':
		pls ? jtableP_set(&opers, u) : jtableP_unset(&opers, u);
		break;
	case 'h':
		pls ? user_setfakehost(u, param) : user_unsetfakehost(u);
		break;
	case 'r':
		pls ? user_setaccount(u, param) : user_unsetaccount(u);
		break;
	}
	return MODEHOOK_OK;
}

void user_apply_mode(struct entity *from, struct user *target, char *modechanges, struct manyargs *arg, int skip) {
	if (!verify_user(target)) {
		logtxt(LOG_WARNING, "Usermode change on non-user");
		return;
	}
	mode_apply(from, (struct entity *)target, &target->mode, modechanges, arg, skip, user_modehook);
}

void user_add_channel(struct user *u, struct channel *c) {
	jtableP_set(&u->channels, c);
}

int user_ison(struct user *u, struct channel *c) {
	return jtableP_check(&u->channels, c);
}

int user_isoper(struct user *u) {
	return jtableP_check(&opers, u);
}

void user_del_channel(struct user *u, struct channel *c) {
	if (!verify_user(u) || !verify_channel(c))
		logtxt(LOG_WARNING, "user_del_channel with invalid entities");
	else
		jtableP_unset(&u->channels, c);
}

static void debug_print_userchan(struct channel *c, struct user *u) {
	logfmt(LOG_DEBUG, "    %s%s%s", channel_isop(c, u) ? "@" : " ", channel_isvoice(c, u) ? "+" : " ", c->name);
}

static void debug_print_user(char *name, struct user *u) {
	if (!verify_user(u)) {
		logtxt(LOG_DEBUG, "INVALID USER");
		return;
	}
	logfmt(LOG_DEBUG, "User info for '%s' == '%s'", name, u->numeric);
	logfmt(LOG_DEBUG, "  Host: %s!%s@%s`%s * %s", u->nick, u->user, u->host, u->account, u->realname);
	logfmt(LOG_DEBUG, "  Awaymsg: %s", u->awaymsg);
	logtxt(LOG_DEBUG, "  Channel memberships:");
	jtableP_iterate1(&u->channels, (void (*)(void *, void *))debug_print_userchan, u);
}

void debug_print_channels(void) {
	logtxt(LOG_DEBUG, "-------------- User ---------------");
	jtableS_iterate0(&userlist_num, (void (*)(char *, void *))debug_print_user);
}
