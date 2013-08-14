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

#ifndef LUACONTROL_H_
#define LUACONTROL_H_

#define MAX_COMMANDS 32
#define MOD_NAME "lua"
#define MOD_VERSION "0.01"
#define MOD_AUTHOR "Phil (modul8)"

#define BOTNICK "L"
#define BOTIDENT "luaserv"
#define BOTHOSTNAME "plservices.metairc.net"
#define BOTMODES "+okr"
#define BOTREALNAME "Lua Control Service"
#define BOTDEBUGCHAN "#labspace3"

/* Structs */
struct commandInfo {
	char *trigger;
	char *syntax;
	char *description;
	cmdfunc cmdptr;
};

struct user *bot;
struct commandInfo commands[MAX_COMMANDS];

/* Functions */
void onprivmsg(struct user *from, struct user *to, char *msg);
cmdfunc command_find(char *trigger);
void command_register(char *trigger, char *syntax, char *description, cmdfunc command);
void commands_initialize(void);
/* Commands */
int command_version(struct user *from, struct user *to, struct manyargs args);
int command_showcommands (struct user *from, struct user *to, struct manyargs args);
#endif
