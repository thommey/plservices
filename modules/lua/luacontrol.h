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

struct commandInfo {
	char *trigger;
	char *syntax;
	char *description;
	cmdfunc cmdptr;
};

struct commandInfo commands[MAX_COMMANDS];

static void onprivmsg(struct user *from, struct user *to, char *msg);
cmdfunc command_find(char *trigger);

#endif
