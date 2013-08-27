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
#include "luabase.h"

static jtableP luabase_states;
static jtableL luabase_users;
static jtableS luabase_states_by_script;

extern time_t now;
extern struct server *me;

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
	const struct luaL_Reg *luafuncs;
	int i;

	L = luaL_newstate();
	luaL_openlibs(L);

	luafuncs = luafuncs_functable();

	for (i = 0; luafuncs[i].func; i++)
		lua_register(L, luafuncs[i].name, luafuncs[i].func);

	jtableP_set(&luabase_states, L);
	int err;
	err = luaL_loadfile(L, config_get("mod.lua", "base_script"));
	if (!err)
		err = lua_pcall(L, 0, 0, 0);
	if (err) {
		luabase_report(L, "initialization", err);
		return NULL;
	}
	logtxt(LOG_ERROR, "Loaded base class.");
	return L;
}

/* load a script in a seperate interpreter instance */
int luabase_loadscript(char *file) {
	struct args arg;
	arg.c = 1;
	arg.v[0] = arg_str("onload");
	lua_State *L = luabase_newstate();
	int err;
	logfmt(LOG_DEBUG, "Loading lua script %s", file);
	err = luaL_loadfile(L, file);
	if (!err)
		err = lua_pcall(L, 0, 0, 0);
	if (err) {
		luabase_report(L, "initialization", err);
		return 1;
	}
	jtableS_insert(&luabase_states_by_script, file, L);
	logfmt(LOG_DEBUG, "Loaded lua script %s", file);
	return luabase_callluafunc(L, &arg);
}

int luabase_unloadscript(char *file) {
	lua_State *L;
	L = luabase_get_interpreter(file);
	logfmt(LOG_DEBUG, "Unloading lua script %s", file);
	struct args arg = pack_words("onunload");
	luabase_callluafunc(L, &arg);
	lua_close(L);
	jtableS_remove(&luabase_states_by_script, file);
	jtableP_unset(&luabase_states, L);
	return 0;
}

int luabase_callluafunc(lua_State *L, struct args *arg) {
	int err, i;

	lua_getglobal(L, argdata_str(&arg->v[0]));

	for (i = 1; i < arg->c; i++)
		lua_pushstring(L, argdata_str(&arg->v[i]));

	err = lua_pcall(L, arg->c - 1, 0, 0);
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
		lua_pushstring(lc->L, argdata_str(&arg->v[i]));

	err = lua_pcall(lc->L, arg->c, 0, 0);
	if (luabase_report(lc->L, "client hook", err))
		return 1;
	logfmt(LOG_LUA, "Successfully called client hook %s for numeric %s", argdata_str(&arg->v[1]), argdata_str(&arg->v[0]));
	return 0;
}

static void luabase_chanhook(unsigned long numeric, struct luaclient *lc, struct args *arg) {
	int i;
	struct args myarg;
	struct user *u = get_user_by_numeric(numeric);
	struct channel *c = argdata_chan(&arg->v[0]);

	if (!u || !c || !chanusers_ison(u, c))
		return;

	myarg = pack_words(u->numericstr, argdata_str(&arg->v[1]));
	for (i = 2; i < arg->c; myarg.c++, i++) {
		myarg.v[myarg.c] = arg->v[i];
	}
	luabase_clienthook(lc, &myarg);
}

static void luabase_ontick(void) {
	struct args arg;
	arg.c = 1;
	arg.v[0] = arg_str("ontick");
	jtableP_iterate(&luabase_states, (jtableP_cb)luabase_callluafunc, &arg);
	arg.v[0] = arg_str("ontick2");
	jtableP_iterate(&luabase_states, (jtableP_cb)luabase_callluafunc, &arg);
}

static void luabase_onprivmsg(struct user *from, struct user *to, char *msg) {
	struct args arg;
	struct luaclient *lc = jtableL_get(&luabase_users, to->numeric);

	if (!lc)
		return;

	arg = pack_words(to->numericstr, "irc_onmsg", from->numericstr, msg);
	luabase_clienthook(lc, &arg);
}

static void luabase_onprivnotc(struct user *from, struct user *to, char *msg) {
	struct args arg;
	struct luaclient *lc = jtableL_get(&luabase_users, to->numeric);

	if (!lc)
		return;

	arg = pack_words(to->numericstr, "irc_onnotice", from->numericstr, msg);
	luabase_clienthook(lc, &arg);
}

static void luabase_onchanmsg(struct user *from, struct channel *chan, char *msg) {
	struct args arg = pack_args(arg_chan(chan), arg_str("irc_onchanmsg"), arg_str(from->numericstr), arg_str(chan->name), arg_str(msg));
	jtableL_iterate(&luabase_users, (jtableL_cb)luabase_chanhook, &arg);
}

static void luabase_onquit(struct user *from, char *msg) {
	struct args arg = pack_args(arg_str("irc_onquit"), arg_str(from->numericstr));
	jtableP_iterate(&luabase_states, (jtableP_cb)luabase_callluafunc, &arg);
}

static void luabase_onpart(struct user *from, struct channel *chan, char *msg) {
	struct args arg = pack_args(arg_str("irc_onpart"), arg_str(chan->name), arg_str(from->numericstr), arg_str(msg));
	jtableP_iterate(&luabase_states, (jtableP_cb)luabase_callluafunc, &arg);
}

int luabase_load(void) {
	//luabase_loadscript("labspace.lua");
	hook_hook("ontick", luabase_ontick);
	hook_hook("onchanmsg", luabase_onchanmsg);
	hook_hook("onquit", luabase_onquit);
	hook_hook("onpart", luabase_onpart);
	hook_hook("onprivmsg", luabase_onprivmsg);
	hook_hook("onprivnotc", luabase_onprivnotc);
	return 0;
}

int luabase_unload(void) {
	/* TODO */
	return 0;
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
	lua_pushstring(L, u->numericstr);
	lua_setfield(L, -2, "numeric");
	lua_pushnumber(L, u->accountid);
	lua_setfield(L, -2, "accountid");
}

void luabase_pushuser_iter(struct user *u, struct luapushuserdata *lpud) {
	lua_pushnumber(lpud->L, (*lpud->i)++);
	luabase_pushuser(lpud->L, u);
	lua_settable(lpud->L, -3);
}

struct user *luabase_newuser(lua_State *L, const char *nick, const char *user, const char *host, const char *umode, const char *account, const char *realname, int handlerref) {
	struct luaclient *lc;
	struct user *u;
	unsigned long numeric;

	numeric = server_freenum(me);
	lc = zmalloc(sizeof(*lc));
	lc->L = L;
	lc->handler_ref = handlerref;
	jtableL_insert(&luabase_users, numeric, lc);

	send_format("%s N %s %d %ld %s %s %s%s%s %s %s :%s", ME, nick, 1, now, user, host, umode, account ? " " : "", account ? account : "", "DAqAoB", unum2str(numeric), realname);
	u = get_user_by_numeric(numeric);
	assert(u);
	return u;
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

lua_State *luabase_get_interpreter(const char *script) {
	return jtableS_get(&luabase_states_by_script, script);
}

int luabase_valid_script(const char *script) {
	if (jtableS_get(&luabase_states_by_script, script) != NULL) {
		return 1;
	}
	return 0;
}
