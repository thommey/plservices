/**
 * hooks.c - Event handling and distribution
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

#include "main.h"

static jtableS hooks;

static void hook_register(char *name) {
	jtableP *t = zmalloc(sizeof(*t));
	jtableS_insert(&hooks, name, t);
}

void init_hooks(void) {
	/* ontick hook, called (ideally) every second, arguments: none */
	hook_register("ontick");
	/* onregistered hook, called after uplink and burst, arguments: none */
	hook_register("onregistered");
	/* onchanmsg hook, called on a PRIVMSG to channel, arguments: struct user *from, struct channel *to, char *msg */
	hook_register("onchanmsg");
	/* onprivmsg hook, called on a PRIVMSG to my user, arguments: struct user *from, struct user *to, char *msg */
	hook_register("onprivmsg");
	/* onnotice hook, called on a NOTICE to my user, arguments: struct user *from, struct user *to, char *msg */
	hook_register("onprivnotc");
}

static jtableP *get_funcs(const char *name) {
	jtableP *funcs = jtableS_get(&hooks, name);
	if (!funcs) {
		logfmt(LOG_WARNING, "Invalid hook '%s'", name);
		return NULL;
	}
	return funcs;
}

void hook_hook(const char *name, void (*f)()) {
	jtableP *funcs = get_funcs(name);
	if (!funcs)
		return;
	jtableP_set(funcs, f);
}

void hook_unhook(const char *name, void (*f)()) {
	jtableP *funcs = get_funcs(name);
	if (!funcs)
		return;
	jtableP_unset(funcs, f);
}

static void hook_call1(void (*func)(), struct args *arg) {
	logtxt(LOG_DEBUG, "  Calling hookfunc");
	call_varargs(func, arg);
}

void hook_call(const char *name, struct args arg) {
	jtableP *funcs = get_funcs(name);

	if (!funcs)
		return;

	logfmt(LOG_DEBUG, "Calling hook %s", name);

	jtableP_iterate(funcs, (jtableP_cb)hook_call1, &arg);
}
