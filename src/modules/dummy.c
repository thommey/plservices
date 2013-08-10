#define MODNAME "dummy"

#define BOTNICK "D"
#define BOTIDENT "dummy"
#define BOTHOSTNAME "plservices.metairc.net"
#define BOTMODES "+okr"
#define BOTREALNAME "Dummy Module"
#define BOTNUMERIC "GEAZA"
#define BOTDEBUGCHAN "#labspace"

#include <stdio.h>
#include "main.h"

extern time_t now;
extern struct server *me;

void onprivmsg(struct user *from, struct user *to, char *msg);

int load() {
	module_create_client(BOTNICK, BOTIDENT, BOTHOSTNAME, BOTMODES, BOTNICK, BOTNICK, BOTNUMERIC, BOTREALNAME);
	module_join_channel(BOTNUMERIC, BOTDEBUGCHAN, 1);
	module_describe(BOTNUMERIC, BOTDEBUGCHAN, "wiggles");
	hook_hook("onprivmsg", onprivmsg);
	logfmt(LOG_DEBUG, "(%s): Loaded.", MODNAME);
	return 0;
}

int unload() { 
	module_destroy_client(BOTNUMERIC, "Module unloaded.");
	logfmt(LOG_DEBUG, "(%s): Unloaded.", MODNAME);
	return 0;
}

void onprivmsg(struct user *from, struct user *to, char *msg) { 
	module_privmsg(BOTNUMERIC, BOTDEBUGCHAN, "I've just received a message from: %s - message: %s", from->nick, msg);
}
