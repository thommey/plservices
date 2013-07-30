/**
 * lua.c - Base of lua integration and helper functions
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

static jtable luabase_states, luabase_users;

extern time_t now;
extern struct server *me;

/* base libraries to inject into every interpreter */
static const luaL_reg luabase_libs[] = {
  { "base",       luaopen_base },
  { "string",     luaopen_string },
  { "table",      luaopen_table },
  { "math",       luaopen_math },
  { NULL,         NULL }
};

int luabase_report(lua_State *L, char *where, int status) {
  const char *msg;
  if (status) {
    msg = lua_tostring(L, -1);
    if (msg == NULL)
      msg = "(error with no message)";
    logfmt(LOG_ERROR, "LUA error @ %s: lua status=%d, %s", where, status, msg);
    lua_pop(L, 1);
  }
  return status;
}

/* create a new lua interpteter */
lua_State *luabase_newstate(void) {
	lua_State *L;
	const luaL_reg *lib;
	const struct luaL_reg *luafuncs;
	int i;

	L = lua_open();

	for (lib = luabase_libs; lib->func != NULL; lib++) {
		lib->func(L);
		lua_settop(L, 0);
	}

	luafuncs = luafuncs_functable();

	for (i = 0; luafuncs[i].func; i++)
		lua_register(L, luafuncs[i].name, luafuncs[i].func);

	jtableP_set(&luabase_states, L);
	return L;
}

/* load a script in a seperate interpreter instance */
int luabase_loadscript(lua_State *L, char *file) {
	int err;
	logfmt(LOG_DEBUG, "Loading lua script %s", file);
	err = luaL_loadfile(L, file);
	if (!err)
		err = lua_pcall(L, 0, 0, 0);
	if (err) {
		luabase_report(L, "initialization", err);
		return 1;
	}
	logfmt(LOG_DEBUG, "Loaded lua script %s", file);
	return 0;
}

static int luabase_callluafunc(lua_State *L, char *func) {
	int err;

	lua_getglobal(L, func);
	err = lua_pcall(L, 0, 0, 0);
	if (err) {
		luabase_report(L, "calling lua function from C", err);
		return 1;
	}
	return 0;
}

static int luabase_clienthook(struct luaclient *lc, struct args *arg) {
	int err, i;

	lua_rawgeti(lc->L, LUA_REGISTRYINDEX, lc->handler_ref);

	for (i = 0; i < arg->c; i++)
		lua_pushstring(lc->L, argdata_ptr(&arg->v[i]));

	err = lua_pcall(lc->L, arg->c, 0, 0);
	if (luabase_report(lc->L, "client hook", err))
		return 1;
	logfmt(LOG_LUA, "Successfully called client hook %s for numeric %s", (char *)argdata_ptr(&arg->v[1]), lc->numeric);
	return 0;
}

static void luabase_chanhook(char *numeric, struct luaclient *lc, struct args *arg) {
	int i;
	struct args myarg;
	struct user *u = get_user_by_numeric(lc->numeric);
	struct channel *c = argdata_ptr(&arg->v[0]);

	if (!u || !c || !chanusers_ison(u, c))
		return;

	myarg = pack(ARGTYPE_PTR, lc->numeric, ARGTYPE_PTR, argdata_ptr(&arg->v[1]));
	for (i = 2; i < arg->c; myarg.c++, i++) {
		myarg.v[myarg.c].type = ARGTYPE_PTR;
		myarg.v[myarg.c].data.p = argdata_ptr(&arg->v[i]);
	}
	luabase_clienthook(lc, &myarg);
}

static void luabase_ontick(void) {
	jtableP_iterate(&luabase_states, (jtableP_cb)luabase_callluafunc, "ontick");
	jtableP_iterate(&luabase_states, (jtableP_cb)luabase_callluafunc, "ontick2");
}

static void luabase_onprivmsg(struct user *from, struct user *to, char *msg) {
	struct args arg;
	struct luaclient *lc = jtableS_get(&luabase_users, to->numeric);

	if (!lc)
		return;

	arg = pack(ARGTYPE_PTR, to->numeric, ARGTYPE_PTR, "irc_onmsg", ARGTYPE_PTR, from->numeric, ARGTYPE_PTR, msg);
	luabase_clienthook(lc, &arg);
}

static void luabase_onprivnotc(struct user *from, struct user *to, char *msg) {
	struct args arg;
	struct luaclient *lc = jtableS_get(&luabase_users, to->numeric);

	if (!lc)
		return;

	arg = pack(ARGTYPE_PTR, to->numeric, ARGTYPE_PTR, "irc_onnotice", ARGTYPE_PTR, from->numeric, ARGTYPE_PTR, msg);
	luabase_clienthook(lc, &arg);
}

static void luabase_onchanmsg(struct user *from, struct channel *chan, char *msg) {
	struct args arg = pack(ARGTYPE_PTR, chan, ARGTYPE_PTR, "irc_onchanmsg", ARGTYPE_PTR, from->numeric, ARGTYPE_PTR, chan->name, ARGTYPE_PTR, msg);
	jtableS_iterate(&luabase_users, (jtableS_cb)luabase_chanhook, &arg);
}

void luabase_init() {
	lua_State *L = luabase_newstate();
	luabase_loadscript(L, "stubs.lua");
	luabase_loadscript(L, "labspace.lua");
	hook_hook("ontick", luabase_ontick);
	hook_hook("onchanmsg", luabase_onchanmsg);
	hook_hook("onprivmsg", luabase_onprivmsg);
	hook_hook("onprivnotc", luabase_onprivnotc);

	luabase_callluafunc(L, "onload");
}

char *nextnum(void) {
	static char numeric[6] = "";
	if (!numeric[0])
		sprintf(numeric, "%s%s", ME, base64_encode_padded(0, numeric + 2, 4));
	else if (base64_incr(numeric + 2, 4))
		error("No numerics left");
	return numeric;
}

/* push userinfo (.nick, .numeric, .accountid) */
void luabase_pushuser(lua_State *L, struct user *u) {
	if (!u) {
		lua_pushnil(L);
		return;
	}
	lua_newtable(L);
	lua_pushstring(L, u->nick);
	lua_setfield(L, -2, "nick");
	lua_pushstring(L, u->numeric);
	lua_setfield(L, -2, "numeric");
	lua_pushnumber(L, u->accountid);
	lua_setfield(L, -2, "accountid");
}

void luabase_pushuser_iter(struct user *u, struct luapushuserdata *lpud) {
	lua_pushnumber(lpud->L, (*lpud->i)++);
	luabase_pushuser(lpud->L, u);
	lua_settable(lpud->L, -3);
}

struct luaclient *luabase_newuser(lua_State *L, const char *nick, const char *user, const char *host, const char *umode, const char *account, const char *realname, int handlerref) {
	struct luaclient *lc;
	char *numeric;

	numeric = nextnum();
	lc = zmalloc(sizeof(*lc));
	lc->L = L;
	strbufcpy(lc->numeric, numeric);
	lc->handler_ref = handlerref;
	jtableS_insert(&luabase_users, numeric, lc);

	send_format("%s N %s %d %ld %s %s %s%s%s %s %s :%s", ME, nick, 1, now, user, host, umode, account ? " " : "", account ? account : "", "DAqAoB", numeric, realname);
	return lc;
}

int luabase_getbooleanfromarray(lua_State *L, int tableidx, int idx) {
	int result;

	lua_pushinteger(L, idx);
	lua_gettable(L, tableidx > 0 ? tableidx : tableidx - 1);
	if (!lua_isboolean(L, -1))
		return luaL_error(L, "Table element is not boolean");
	result = lua_toboolean(L, -1);
	lua_pop(L, 1);
	return result;
}

int luabase_getintfromarray(lua_State *L, int tableidx, int idx) {
	int result;

	lua_pushinteger(L, idx);
	lua_gettable(L, tableidx > 0 ? tableidx : tableidx - 1);
	result = luaL_checkint(L, -1);
	lua_pop(L, 1);
	return result;
}

const char *luabase_getstringfromarray(lua_State *L, int tableidx, int idx) {
	const char *result;

	lua_pushinteger(L, idx);
	lua_gettable(L, tableidx > 0 ? tableidx : tableidx - 1);
	result = luaL_checkstring(L, -1);
	lua_pop(L, 1);
	return result;
}
