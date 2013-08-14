#include "main.h"
#include "luabase.h"
#include "luacontrol.h"
#include <string.h>

int command_version (struct user *from, struct user *to, struct manyargs args) {
        module_notice(bot, from->numericstr, "VERSION The Lua Control Service v%s by %s", MOD_VERSION, MOD_AUTHOR);
        module_notice(bot, from->numericstr, "VERSION Built on plservices by thommey");
        return 0;
}

int command_showcommands (struct user *from, struct user *to, struct manyargs args) {
        for (int x = 0; x<MAX_COMMANDS; x++) { 
		if (commands[x].trigger != NULL) { 
			char *command = strtoupper(commands[x].trigger);
			module_notice(bot, from->numericstr, "%s %s", command, commands[x].description);
			free(command);
		}
	}
	module_notice(bot, from->numericstr, "End of list.");
	return 0;
}
