/**
 * luafuncs.c - All C functions exported to lua
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

/* Include the Lua API header files. */
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "main.h"

extern struct server *me;
extern time_t now;

static int luafunc_os_time(lua_State *L) {
	lua_pushnumber(L, now);
	return 1;
}

static int luafunc_irc_localregisteruser(lua_State *L) {
	struct luaclient *lc;
	const char *nick = luaL_checkstring(L, 1);
	const char *user = luaL_checkstring(L, 2);
	const char *host = luaL_checkstring(L, 3);
	const char *realname = luaL_checkstring(L, 4);
	const char *umode = luaL_checkstring(L, 6);
	luaL_checktype(L, -1, LUA_TFUNCTION);

	lc = luabase_newuser(L, nick, user, host, umode, NULL, realname, luaL_ref(L, LUA_REGISTRYINDEX));
	lua_pushstring(L, lc->numeric);
	return 1;
}

static int luafunc_irc_localregisteruserid(lua_State *L) {
	struct luaclient *lc;
	const char *nick = luaL_checkstring(L, 1);
	const char *user = luaL_checkstring(L, 2);
	const char *host = luaL_checkstring(L, 3);
	const char *realname = luaL_checkstring(L, 4);
	const char *account = luaL_checkstring(L, 5);
	const char *umode = luaL_checkstring(L, 7);
	luaL_checktype(L, -1, LUA_TFUNCTION);

	lc = luabase_newuser(L, nick, user, host, umode, account, realname, luaL_ref(L, LUA_REGISTRYINDEX));
	lua_pushstring(L, lc->numeric);
	return 1;
}

static int luafunc_irc_localchanmsg(lua_State *L) {
	const char *numeric = luaL_checkstring(L, 1);
	const char *chan = luaL_checkstring(L, 2);
	const char *msg = luaL_checkstring(L, 3);

	send_words(1, numeric, "P", chan, msg);
	return 0;
}

static int luafunc_irc_getnickbynumeric(lua_State *L) {
	const char *numeric = luaL_checkstring(L, 1);

	luabase_pushuser(L, get_user_by_numeric(numeric));
	return 1;
}

static int luafunc_irc_getnickbynick(lua_State *L) {
	const char *nick = luaL_checkstring(L, 1);

	luabase_pushuser(L, get_user_by_nick(nick));
	return 1;
}

static int luafunc_irc_localovmode(lua_State *L) {
	const char *numeric = luaL_checkstring(L, 1);
	const char *chan = luaL_checkstring(L, 2);
	struct user *u = get_user_by_numeric(numeric);
	struct channel *c = get_channel_by_name(chan);
	const char *modechar, *target; /* pointers for luaL_getstring later */
	int plsmns, i, n = lua_objlen(L, 3); /* n: list length */
	struct modebuf *modebuf = NULL;

	if (!u)
		return luaL_error(L, "User does not exist on the network: %s", numeric);
	if (!c)
		return luaL_error(L, "Channel does not exist on the network: %s", chan);
	if (n % 3)
		return luaL_error(L, "Invalid mode table format");

	/* silently throw away modes from non-ops */
	if (!channel_isop(c, u))
		return 0;

	/* 3 element strides (plsmns, modechar, target numeric) */
	for (i = 0; i < n; i += 3) {
		/* lua list counting starts at 1 => +1 all the things */
		plsmns = luabase_getbooleanfromarray(L, -1, i + 1);
		modechar = luabase_getstringfromarray(L, -1, i + 2);
		target = luabase_getstringfromarray(L, -1, i + 3);
		modebuf = mode_pushmode(u, c, plsmns, modechar[0], target, strlen(target));
	}
	mode_flushmode(modebuf);
	return 0;
}

static int luafunc_irc_localjoin(lua_State *L) {
	struct channel *c;
	const char *num = luaL_checkstring(L, 1);
	const char *chan = luaL_checkstring(L, 2);

	c = get_channel_by_name(chan);

	if (c) {
		send_format("%s J %s %ld", num, chan, c->ts);
		send_format("%s M %s +o %s", ME, chan, num);
	} else
		send_format("%s C %s %ld", num, chan, now);
	return 0;
}

static int luafunc_irc_localsimplechanmode(lua_State *L) {
	const char *unum = luaL_checkstring(L, 1);
	const char *chan = luaL_checkstring(L, 2);
	const char *mode = luaL_checkstring(L, 3);
	struct user *u = get_user_by_numeric(unum);
	struct channel *c = get_channel_by_name(chan);

	if (!u || !c || strlen(mode) != 2 || (mode[0] != '+' && mode[0] != '-'))
		luaL_error(L, "Invalid modechange: %s %s %s", chan, mode, unum);

	mode_pushmode(u, c, mode[0] == '+', mode[1], NULL, 0);
	return 0;
}

static int luafunc_irc_channeluserlist(lua_State *L) {
	int i = 1;
	struct luapushuserdata lpud;
	const char *chan = luaL_checkstring(L, 1);
	struct channel *c = get_channel_by_name(chan);

	lpud.L = L;
	lpud.i = &i;
	if (!c) {
		lua_pushnil(L);
		return 1;
	}
	lua_newtable(L);
	jtableP_iterate(&c->users, (jtableP_cb)luabase_pushuser_iter, &lpud);
	return 1;
}

static int luafunc_irc_localnotice(lua_State *L) {
	const char *unum = luaL_checkstring(L, 1);
	const char *target = luaL_checkstring(L, 2);
	const char *msg = luaL_checkstring(L, 3);

	send_words(1, unum, "O", target, msg);
	return 0;
}

static int luafunc_irc_getchaninfo(lua_State *L) {
	const char *chan = luaL_checkstring(L, 1);
	lua_pushinteger(L, get_channel_by_name(chan) ? 1 : 0);
	return 1;
}

static int luafunc_irc_getuserchanmodes(lua_State *L) {
	char *unum = (char*)luaL_checkstring(L, 1);
	char *chan = (char*)luaL_checkstring(L, 2);
	struct user *u = get_user_by_numeric(unum);
	struct channel *c = get_channel_by_name(chan);

	if (!u || !c || !chanusers_ison(u, c)) {
		lua_pushnil(L);
		return 1;
	}
	lua_newtable(L);
	if (channel_isop(c, u)) {
		lua_pushinteger(L, 1);
		lua_setfield(L, -2, "opped");
	}
	if (channel_isop(c, u)) {
		lua_pushinteger(L, 1);
		lua_setfield(L, -2, "voiced");
	}

	return 1;
}

static int luafunc_irctolower(lua_State *L) {
	static char buf[513];
	const char *src = luaL_checkstring(L, 1);

	rfc_tolower(buf, sizeof(buf), src);

	lua_pushstring(L, buf);
	return 1;
}

static int luafunc_ontlz(lua_State *L) {
	const char *num = luaL_checkstring(L, 1);
	struct user *u = get_user_by_numeric(num);

	lua_pushinteger(L, u ? user_isoper(u) : 0);
	return 1;
}

static int luafunc_onstaff(lua_State *L) {
	return luafunc_ontlz(L);
}

#define MKLFUNC(name) { # name , luafunc_ ## name }

static const struct luaL_reg luafuncs[] = {
	MKLFUNC(irc_localregisteruser),
	MKLFUNC(irc_localregisteruserid),
	MKLFUNC(irc_localchanmsg),
	MKLFUNC(irc_getnickbynumeric),
	MKLFUNC(irc_getnickbynick),
	MKLFUNC(irc_localovmode),
	MKLFUNC(irc_localjoin),
	MKLFUNC(irc_localsimplechanmode),
	MKLFUNC(irc_channeluserlist),
	MKLFUNC(irc_localnotice),
	MKLFUNC(irc_getuserchanmodes),
	MKLFUNC(irc_getchaninfo),
	MKLFUNC(irctolower),
	MKLFUNC(onstaff),
	MKLFUNC(ontlz),
	MKLFUNC(os_time),
	{ NULL, NULL }
};

const struct luaL_reg *luafuncs_functable(void) {
	return luafuncs;
};
