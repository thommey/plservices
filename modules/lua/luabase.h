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
 */

#ifndef LUA_H_
#define LUA_H_

#include <lua.h>

struct luaclient {
	lua_State *L;
	int handler_ref;
};

struct luapushuserdata {
	lua_State *L;
	int *i;
};

int luabase_load(void);
int luabase_unload(void);
void luabase_ghook(char *str, struct args *arg);
struct user *luabase_newuser(lua_State *L, const char *nick, const char *user, const char *host, const char *umode, const char *account, const char *realname, int handlerref);
void luabase_pushuser(lua_State *L, struct user *u);
void luabase_pushuser_iter(struct user *u, struct luapushuserdata *lpud);
int luabase_getbooleanfromarray(lua_State *L, int tableidx, int idx);
int luabase_getintfromarray(lua_State *L, int tableidx, int idx);
const char *luabase_getstringfromarray(lua_State *L, int tableidx, int idx);
lua_State *luabase_get_interpreter(const char *script);
int luabase_loadscript(char *file);
int luabase_unloadscript(char *file);
int luabase_valid_script(const char *script);
int luabase_callluafunc(lua_State *L, struct args *arg);

const struct luaL_reg *luafuncs_functable(void);

#endif /* LUA_H_ */
