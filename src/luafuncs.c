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
**/

/* Include the Lua API header files. */
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "main.h"

extern time_t now;
extern struct server *me;

static int lua_now(lua_State *L) {
	lua_pushnumber(L, now);
	return 1;
}

static int lua_localregisteruser(lua_State *L) {
	struct luaclient *lc;
	const char *nick = luaL_checkstring(L, 1);
	const char *user = luaL_checkstring(L, 2);
	const char *host = luaL_checkstring(L, 3);
	const char *realname = luaL_checkstring(L, 4);
	const char *umode = luaL_checkstring(L, 6);
	luaL_checktype(L, -1, LUA_TFUNCTION);

	lc = lua_newuser(L, nick, user, host, umode, NULL, realname, luaL_ref(L, LUA_REGISTRYINDEX));
	lua_pushstring(L, lc->numeric);
	return 1;
}

static int lua_localregisteruserid(lua_State *L) {
	struct luaclient *lc;
	const char *nick = luaL_checkstring(L, 1);
	const char *user = luaL_checkstring(L, 2);
	const char *host = luaL_checkstring(L, 3);
	const char *realname = luaL_checkstring(L, 4);
	const char *account = luaL_checkstring(L, 5);
	const char *umode = luaL_checkstring(L, 7);
	luaL_checktype(L, -1, LUA_TFUNCTION);

	lc = lua_newuser(L, nick, user, host, umode, account, realname, luaL_ref(L, LUA_REGISTRYINDEX));
	lua_pushstring(L, lc->numeric);
	return 1;
}

static int lua_localchanmsg(lua_State *L) {
	const char *numeric = luaL_checkstring(L, 1);
	const char *chan = luaL_checkstring(L, 2);
	const char *msg = luaL_checkstring(L, 3);

	send_words(1, numeric, "P", chan, msg);
	return 0;
}

static int lua_getnickbynumeric(lua_State *L) {
	char *numeric = (char *)luaL_checkstring(L, 1);

	lua_pushuser(L, get_user_by_numeric(numeric));
	return 1;
}

static int lua_getnickbynick(lua_State *L) {
	const char *nick = luaL_checkstring(L, 1);

	lua_pushuser(L, get_user_by_nick(nick));
	return 1;
}

static int lua_localmode(lua_State *L) {
	const char *numeric = luaL_checkstring(L, 1);
	const char *chan = luaL_checkstring(L, 2);
	const char *modestr = luaL_checkstring(L, 3);

	send_format("%s M %s %s", numeric, chan, modestr);
	return 0;
}

static int lua_localjoin(lua_State *L) {
	struct channel *c;
	const char *num = luaL_checkstring(L, 1);
	char *chan = (char *)luaL_checkstring(L, 2);

	c = get_channel_by_name(chan);

	if (c)
		send_format("%s J %s %ld", num, chan, c->ts);
	else
		send_format("%s C %s %ld", num, chan, now);
	return 0;
}

static int lua_localsimplechanmode(lua_State *L) {
	const char *unum = luaL_checkstring(L, 1);
	char *chan = (char *)luaL_checkstring(L, 2);
	const char *mode = luaL_checkstring(L, 3);

	send_words(0, unum, "M", chan, mode);
	return 0;
}

static int lua_channeluserlist(lua_State *L) {
	int i = 0;
	struct luapushuserdata lpud;
	char *chan = (char *)luaL_checkstring(L, 1);
	struct channel *c = get_channel_by_name(chan);

	lpud.L = L;
	lpud.i = &i;
	if (!c) {
		lua_pushnil(L);
		return 1;
	}
	lua_newtable(L);
	jtableP_iterate1(&c->users, (void (*)(void *, void*))lua_pushuser_iter, &lpud);
	return 1;
}

static int lua_localnotice(lua_State *L) {
	const char *unum = luaL_checkstring(L, 1);
	const char *target = luaL_checkstring(L, 2);
	const char *msg = luaL_checkstring(L, 3);

	send_words(1, unum, "O", target, msg);
	return 0;
}

static int lua_getchaninfo(lua_State *L) {
	char *chan = (char *)luaL_checkstring(L, 1);
	lua_pushinteger(L, get_channel_by_name(chan) ? 1 : 0);
	return 1;
}

static int lua_getuserchanmodes(lua_State *L) {
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

static int lua_ontlz(lua_State *L) {
	char *num = (char *)luaL_checkstring(L, 1);
	struct user *u = get_user_by_numeric(num);

	logfmt(LOG_LUA, "ontlz? %d", u ? user_isoper(u) : 0);
	lua_pushinteger(L, u ? user_isoper(u) : 0);
	return 1;
}

static int lua_onstaff(lua_State *L) {
	return lua_ontlz(L);
}

#define IRCPREFIX(name) "irc_" # name
#define LUAPREFIX(name) lua_ ## name
#define STRINGIFY(x) # x
#define MKLFUNC(name) { IRCPREFIX(name) , LUAPREFIX(name) }

static struct luafunctable luafuncs[] = {
	MKLFUNC(localregisteruser),
	MKLFUNC(localregisteruserid),
	MKLFUNC(localchanmsg),
	MKLFUNC(getnickbynumeric),
	MKLFUNC(getnickbynick),
	MKLFUNC(localmode),
	MKLFUNC(localjoin),
	MKLFUNC(localsimplechanmode),
	MKLFUNC(channeluserlist),
	MKLFUNC(localnotice),
	MKLFUNC(getuserchanmodes),
	MKLFUNC(now),
	MKLFUNC(getchaninfo),
	{ "onstaff", lua_onstaff },
	{ "ontlz", lua_ontlz },
	{ NULL, NULL }
};

struct luafunctable *getluafunctable(void) {
	return luafuncs;
};
