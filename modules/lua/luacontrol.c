#define MODNAME "lua"

#define BOTNICK "L"
#define BOTIDENT "luaserv"
#define BOTHOSTNAME "plservices.metairc.net"
#define BOTMODES "+okr"
#define BOTREALNAME "Lua Control Service"
#define BOTDEBUGCHAN "#labspace3"

#include "main.h"
#include "luabase.h"
#include "luacontrol.h"
#include <string.h>

struct user *bot;

int load() {
	bot = module_create_client(BOTNICK, BOTIDENT, BOTHOSTNAME, BOTMODES, BOTNICK, BOTNICK, BOTREALNAME);
	module_join_channel(bot, BOTDEBUGCHAN, 0);
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

void onprivmsg(struct user *from, struct user *to, char *msg) {
	if (!strcmp(to->numericstr, bot->numericstr)) {
		char message[513];
		struct manyargs arg;
		strbufcpy(message, msg);
		split(&arg, message, ' ');
		cmdfunc cmd = (arg.c == 0 ? NULL : command_find(arg.v[0]));
		if (arg.c == 0 || !command_find(arg.v[0])) { 
			module_notice(bot, from->numericstr, "Unknown command %s. Use SHOWCOMMANDS to view all commands.", arg.v[0]);
		} else { 
			cmd(from, to, &arg);
		}
	}
}


/* Commands */
cmdfunc command_find(char *trigger) {
        for (int x = 0; x < MAX_COMMANDS; x++) {
                if (commands[x].trigger != NULL && strcmp(commands[x].trigger, trigger)) {
                        return commands[x].cmdptr;
                }
        }
	return NULL;
}
