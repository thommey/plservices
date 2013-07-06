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

#ifndef LUA_H_
#define LUA_H_

#include <lua.h>

struct luaclient {
	char numeric[6];
	lua_State *L;
	int handler_ref;
};

struct ghookarg {
	char *name;
	struct args *arg;
};

struct luapushuserdata {
	lua_State *L;
	int *i;
};

void lua_init(void);
void lua_ghook(char *str, struct args *arg);
struct luaclient *lua_newuser(lua_State *L, const char *nick, const char *user, const char *host, const char *umode, const char *account, const char *realname, int handlerref);
void lua_pushuser(lua_State *L, struct user *u);
void lua_pushuser_iter(struct user *u, struct luapushuserdata *lpud);
int lua_getbooleanfromarray(lua_State *L, int tableidx, int idx);
int lua_getintfromarray(lua_State *L, int tableidx, int idx);
const char *lua_getstringfromarray(lua_State *L, int tableidx, int idx);

void lua_clienthook(char *numeric, ...);
void lua_channelhook(char *channel, ...);
#define lua_clienthook(...) lua_clienthook(__VA_ARGS__, NULL)
#define lua_channelhook(...) lua_channelhook(__VA_ARGS__, NULL)

struct luafunctable {
	char *fname;
	int (*f)(lua_State*);
};

struct luafunctable *getluafunctable(void);

#endif // LUA_H_
