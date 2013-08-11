#define MODNAME "dummy"

#define BOTNICK "D"
#define BOTIDENT "dummy"
#define BOTHOSTNAME "plservices.metairc.net"
#define BOTMODES "+okr"
#define BOTREALNAME "Dummy Module"
#define BOTDEBUGCHAN "#labspace"

#include "main.h"

struct user *bot;

static void onprivmsg(struct user *from, struct user *to, char *msg);

int load() {
	bot = module_create_client(BOTNICK, BOTIDENT, BOTHOSTNAME, BOTMODES, BOTNICK, BOTNICK, BOTREALNAME);
	module_join_channel(bot, BOTDEBUGCHAN, 1);
	module_describe(bot, BOTDEBUGCHAN, "wiggles");
	hook_hook("onprivmsg", onprivmsg);
	logfmt(LOG_DEBUG, "(%s): Loaded.", MODNAME);
	return 0;
}

int unload() {
	module_destroy_client(bot, "Module unloaded.");
	logfmt(LOG_DEBUG, "(%s): Unloaded.", MODNAME);
	return 0;
}

static void onprivmsg(struct user *from, struct user *to, char *msg) {
	module_privmsg(bot, BOTDEBUGCHAN, "I've just received a message from: %s - message: %s", from->nick, msg);
}
