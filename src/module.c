/*
 * module.c - Module management.
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

#include <dlfcn.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

extern struct server *me;

int module_register(void *module, const char *name, const char *description) {
	int index = 0;
	for (; modules[index].module != NULL; index++);
	if (index > MAX_MODULES) {
		logfmt(LOG_WARNING, "Cannot load module '%s' - max modules limit exceeded.", name);
		return 1;
	}
	modules[index].module = module;
	modules[index].name = name;
	modules[index].description = description;
	logfmt(LOG_DEBUG, "Module added - index: %d - name: %s - description: %s", index, name, description);
	return 0;
}

int module_unregister(void *module) {
	int position = -1;
	for (int x = 0; x < MAX_MODULES; x++) {
		if (modules[x].module == module) {
			position = x;
			break;
		}
	}
	for (int c = position; c < MAX_MODULES - 1 ; c++) modules[c] = modules[c+1];
	return 1;
}

int module_unload(const char *name) {
	void *mod = module_find_by_name(name);
	if (mod == NULL) return 1;
	module_func unload;
	unload = dlsym(mod, "unload");
	if (dlerror()) {
		logfmt(LOG_ERROR, "Cannot call 'unload' for %s - error: %s", name, dlerror());
		return 1;
	}
	module_unregister(mod);
	int result = unload();
	return result;
}

int module_load(const char *path, const char *name, const char *description) {
	logfmt(LOG_DEBUG, "(%s): Loading...", name);
	void *module;
	module = dlopen(path, RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND);
	if (!module) {
		logfmt(LOG_ERROR, "Cannot load %s - error: %s", name, dlerror());
		return 1;
	}
	module_func init;
	init = dlsym(module, "load");
	if (dlerror()) {
		logfmt(LOG_ERROR, "Cannot call 'load' for %s - error: %s", path, dlerror());
		return 1;
	}
	int res = init();
	if (res > 0) {
		logfmt(LOG_ERROR, "(%s): load() was called, and exited with error.", name);
		return 1;
	}
	module_register(module, name, description);
	return 0;
}

void * module_find_by_name(const char *name) {
	for (int x = 0; x < MAX_MODULES; x++) {
		if (!strncmp(modules[x].name, name, strlen(name))) {
			return modules[x].module;
		}
	}
	return NULL;
}

const char *module_get_description(const char *name) {
	for (int x = 0; x < MAX_MODULES; x++) {
		if (!strncmp(modules[x].name, name, strlen(name))) {
			return modules[x].description;
		}
	}
	return NULL;
}

void module_loadAll() {
	module_load("modules/lua.so", "lua", "PLServices Lua Module.");
	module_load("modules/tcl.so", "tcl", "PLServices TCL Module.");
}


/* Module helper functions */

extern struct server *me;
extern time_t now;

/*
* @function: Makes a fake client join a channel, if it exists, or create it if it doesn't.
* @return: void
*/
void module_join_channel(struct user *from, const char *channel, int auto_op) {
	struct channel *chan = get_channel_by_name(channel);
	if (chan == NULL) {
		// Channel doesn't exist - lets create it.
		send_format("%s C %s %ld", from->numericstr, channel, now);
	} else {
		send_format("%s J %s %ld", from->numericstr, chan->name, chan->ts);
		if (auto_op) {
			send_format("%s M %s +o %s", me->numericstr, chan->name, from->numericstr);
		}
	}
}

/*
* @function: Makes a fakeclient part a channel.
* @return: void
*/
void module_part_channel(struct user *from, const char *channel) {
	// Don't try to part a channel that doesn't exist :)
	if (get_channel_by_name(channel) == NULL) return;
	send_format("%s L %s", from->numericstr, channel);
}

/*
* @function: Sends an action to a channel.
* @return: void
*/

void module_describe(struct user *from, const char *target, const char *message, ...) {
	static char buf[512];
	va_list ap;
	va_start(ap, message);
	vsprintf(buf, message, ap);
	va_end(ap);
	// Don't send to a non-valid target, kthx.
	if (get_channel_by_name(target) == NULL && get_user_by_numericstr(target) == NULL) return;
	send_format("%s P %s :\001ACTION %s\001", from->numericstr, target, buf);
}

/*
* @function: Sends an action to a channel.
* @return: void
*/
void module_privmsg(struct user *from, const char *target, const char *message, ...) {
	static char buf[512];
	va_list ap;
	va_start(ap, message);
	vsprintf(buf, message, ap);
	va_end(ap);
	// Don't send this message to a non valid target, kthx.
	if (get_channel_by_name(target) == NULL && get_user_by_numericstr(target) == NULL) return;
	send_format("%s P %s :%s", from->numericstr, target, buf);
}

/*
* @function: Sends an action to a channel.
* @return: void
*/
void module_notice(struct user *from, const char *target, const char *message, ...) {
        static char buf[512];
        va_list ap;
        va_start(ap, message);
        vsprintf(buf, message, ap);
        va_end(ap);
        // Don't send this message to a non valid target, kthx.
	// who sends a notice to a channel?
        if (get_user_by_numericstr(target) == NULL) return;
        send_format("%s O %s :%s", from->numericstr, target, buf);
}

/*
* @function: Creates a fake client on a specific server
* @return: void
*/
struct user *module_create_client_on(struct server *server, const char *nick, const char *ident, const char *hostname, const char *modes, const char *account, const char *opername, const char *realname) {
	unsigned long numeric = server_freenum(server);
	if (!strlen(account) && strchr(modes, 'r') != NULL) account = nick;
	if (!strlen(opername) && strchr(modes, 'o') != NULL) opername = nick;
	char *line;
	send_format("%s N %s 1 %ld %s %s %s%s%s%s%s %s %s :%s", server->numericstr, nick, now, ident, hostname, modes, (strchr(modes, 'r') == NULL ? "" : " "), (strchr(modes, 'r') == NULL ? "" : account), (strchr(modes, 'o') == NULL ? "" : " "), (strchr(modes, 'o') == NULL ? "" : opername), "B]AAAB", unum2str(numeric), realname);
	return get_user_by_numeric(numeric);
}

/*
* @function: Creates a fake client on the main server
* @return: void
*/
struct user *module_create_client(const char *nick, const char *ident, const char *hostname, const char *modes, const char *account, const char *opername, const char *realname) {
	return module_create_client_on(me, nick, ident, hostname, modes, account, opername, realname);
}

/*
* @function: Destroys a fake client
* @return: void
*/
void module_destroy_client(struct user *from, const char *message) {
	send_format("%s Q :%s", from->numericstr, message);
}
