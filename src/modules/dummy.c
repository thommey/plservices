#define MODNAME "dummy"

#define BOTNICK "D"
#define BOTIDENT "dummy"
#define BOTHOSTNAME "plservices.metairc.net"
#define BOTMODES "+okr D D:0:0:0"
#define BOTREALNAME "Dummy Module"
#define BOTNUMERIC "AZA"
#define BOTDEBUGCHAN "#labspace"

#include <stdio.h>
#include "main.h"

extern time_t now;
extern struct server *me;

void onprivmsg(struct user *from, struct user *to, char *msg);

int load() {
	send_format("%s N %s 1 %ld %s %s %s %s GE%s :%s", "GE", BOTNICK, time(NULL), BOTIDENT, BOTHOSTNAME, BOTMODES, "DAqAoB", BOTNUMERIC, BOTREALNAME);
	send_format("%s%s J %s %ld", "GE", BOTNUMERIC, BOTDEBUGCHAN, time(NULL));
	send_format("GE M %s +o GE%s", BOTDEBUGCHAN, BOTNUMERIC);
	send_format("GE%s P %s :\001ACTION wiggles\001", BOTNUMERIC, BOTDEBUGCHAN);
	hook_hook("onprivmsg", onprivmsg);
	logfmt(LOG_DEBUG, "(%s): Loaded.", MODNAME);
	return 0;
}

int unload() { 
	send_format("GE%s Q :Module unloaded.", BOTNUMERIC);
	logfmt(LOG_DEBUG, "(%s): Unloaded.", MODNAME);
	return 0;
}

void onprivmsg(struct user *from, struct user *to, char *msg) { 
	send_format("GE%s P %s :I've just received a message from: %s - message: %s", BOTNUMERIC, BOTDEBUGCHAN, from->nick, msg);
}
