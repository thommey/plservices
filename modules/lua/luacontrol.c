#define MODNAME "lua"

#define BOTNICK "L"
#define BOTIDENT "luaserv"
#define BOTHOSTNAME "plservices.metairc.net"
#define BOTMODES "+okr"
#define BOTREALNAME "Lua Control Service"
#define BOTDEBUGCHAN "#labspace"

#include "main.h"
#include "luabase.h"
#include "luacontrol.h"
#include <string.h>

struct user *bot;

int load() {
	bot = module_create_client(BOTNICK, BOTIDENT, BOTHOSTNAME, BOTMODES, BOTNICK, BOTNICK, BOTREALNAME);
	module_join_channel(bot, BOTDEBUGCHAN, 1);
	module_describe(bot, BOTDEBUGCHAN, "enters on a chariot of fire.");
	hook_hook("onprivmsg", onprivmsg);
	logfmt(LOG_DEBUG, "(%s): Loaded.", MODNAME);
	return luabase_load();
}

int unload() {
	module_destroy_client(bot, "Module unloaded.");
	logfmt(LOG_DEBUG, "(%s): Unloaded.", MODNAME);
	return luabase_unload();
}

static void onprivmsg(struct user *from, struct user *to, char *msg) {
	struct args argz; 
	struct args *arg;
	argz = pack_words(msg);
	arg = &argz;
	if (command_find(argdata_str(&arg->v[0])) == NULL) { 
		module_notice(bot, from->numericstr, "Unknown command %s. Use SHOWCOMMANDS to view all commands.", argdata_str(&arg->v[0]));
	}
}


/* Commands */
int *command_find(char *trigger) {
        for (int x = 0; x < MAX_COMMANDS; x++) {
                if (!strncmp(commands[x].trigger, trigger, strlen(trigger))) {
                        return commands[x].cmdptr;
                }
        }
	return NULL;
}
