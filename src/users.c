/**
 * users.c - User management
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

static jtableL userlist_num; /* numeric -> struct user* */
static jtableS userlist_nick; /* nick -> struct user* */
static jtableP opers; /* struct user * -> bool */

struct user *get_user(struct user *u) {
	if (!u || !verify_user(u))
		return NULL;
	return u;
}

struct user *get_user_by_numeric(unsigned long numeric) {
	return jtableL_get(&userlist_num, numeric);
}

struct user *get_user_by_numericstr(const char *numeric) {
	return get_user_by_numeric(str2unum(numeric));
}

struct user *get_user_by_nick(const char *nick) {
	char buf[NICKLEN+1];
	return jtableS_get(&userlist_nick, strconv(rfc_tolower, buf, sizeof(buf), nick));
}

struct user *add_user(char *numeric, int hops, char *nick, const char *user, const char *host, const char *realname) {
	char buf[NICKLEN+1];
	struct user *u;
	struct server *s;

	u = zmalloc(sizeof(*u));
	u->magic = MAGIC_USER;
	strbufcpy(u->numericstr, numeric);
	u->numeric = str2unum(numeric);
	u->hops = hops;
	strbufcpy(u->nick, nick);
	strbufcpy(u->user, user);
	strbufcpy(u->host, host);
	strbufcpy(u->realname, realname);

	s = get_server_by_numericstr(numeric);
	assert(s);
	server_add_user(s, u);
	u->server = s;

	jtableS_insert(&userlist_nick, strconv(rfc_tolower, buf, sizeof(buf), nick), u);
	return jtableL_insert(&userlist_num, u->numeric, u);
}

void del_user_iter(void *uptr, void *unused) {
	if (!uptr || !verify_user(uptr))
		return;
	del_user(uptr);
}

void del_user(struct user *user) {
	char buf[NICKLEN+1];
	hook_call("onuserdel", pack_args(arg_ptr(user)));
	server_del_user(user->server, user);
	jtableS_remove(&userlist_nick, strconv(rfc_tolower, buf, sizeof(buf), user->nick));
	jtableL_remove(&userlist_num, user->numeric);
	jtableP_unset(&opers, user);
	free(user);
}

void user_nickchange(struct user *u, char *newnick) {
	char buf[NICKLEN+1];
	jtableS_remove(&userlist_nick, strconv(rfc_tolower, buf, sizeof(buf), u->nick));
	jtableS_insert(&userlist_nick, strconv(rfc_tolower, buf, sizeof(buf), newnick), u);
	strbufcpy(u->nick, newnick);
}

#define ADDSETBUF(name) void user_set ## name (struct user *u, char *str) { if (str) strbufcpy(u->name, str); }
#define ADDUNSETBUF(name) void user_unset ## name (struct user *u) { *u->name = '\0'; }
#define ADDBUF(name) ADDSETBUF(name) ADDUNSETBUF(name)

ADDBUF(opername)
ADDBUF(fakehost)
ADDBUF(awaymsg)

void user_setaccount(struct user *u, char *accname, time_t ts, long id, long flags) {
	strbufcpy(u->account, accname);
	u->acctime = ts;
	u->accid = id;
	u->accflags = flags;
}

static void user_setaccountcolon(struct user *u, char *param) {
	struct manyargs arg;
	char *accname;
	time_t ts;
	long id, flags;

	split(&arg, param, ':');
	if (arg.c == 0) {
		logtxt(LOG_WARNING, "Invalid +r parameter");
		return;
	}
	accname = arg.v[0];
	ts = arg.c > 1 ? (time_t)strtol(arg.v[1], NULL, 10) : 0;
	id = arg.c > 2 ? strtol(arg.v[2], NULL, 10) : 0;
	flags = arg.c > 3 ? strtol(arg.v[3], NULL, 10) : 0;
	user_setaccount(u, accname, ts, id, flags);
}

void user_unsetaccount(struct user *u) {
	u->account[0] = '0';
	u->account[1] = '\0';
	u->acctime = 0;
	u->accflags = 0;
	u->accid = 0;
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
		pls ? user_setaccountcolon(u, param) : user_unsetaccount(u);
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

int user_isoper(struct user *u) {
	return jtableP_check(&opers, u);
}
