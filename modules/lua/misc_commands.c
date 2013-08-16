#include "main.h"
#include "luabase.h"
#include "luacontrol.h"
#include <string.h>

int command_version (struct user *from, struct user *to, struct manyargs *args) {
        module_notice(bot, from->numericstr, "VERSION The Lua Control Service v%s by %s", MOD_VERSION, MOD_AUTHOR);
        module_notice(bot, from->numericstr, "VERSION Built on plservices by thommey");
        return 0;
}

int command_help (struct user *from, struct user *to, struct manyargs *args) { 
	if (args->c == 1) { 
		module_notice(bot, from->numericstr, "Usage: HELP <command>");
		module_notice(bot, from->numericstr, "Shows help such as syntax and descriptions for commands.");
		return 0;
	}
	for (int x = 0; x<MAX_COMMANDS; x++) {
                if (commands[x].trigger != NULL && !strcmp(strtolower(commands[x].trigger), strtolower(args->v[1]))) {
                        char *command = strtoupper(commands[x].trigger);
			module_notice(bot, from->numericstr, "Showing help for command %s", args->v[1]);
                        module_notice(bot, from->numericstr, "Usage: %s", commands[x].syntax);
			module_notice(bot, from->numericstr, "%s", commands[x].description);
                        free(command);
			return 0;
                }
        }
	module_notice(bot, from->numericstr, "No help available for %s.", args->v[0]);
	return 0;
}

int command_showcommands (struct user *from, struct user *to, struct manyargs *args) {
	module_notice(bot, from->numericstr, "Listing all available commands:");
        for (int x = 0; x<MAX_COMMANDS; x++) { 
		if (commands[x].trigger != NULL) { 
			char *command = strtoupper(commands[x].trigger);
			module_notice(bot, from->numericstr, "%s \t %s", command, commands[x].description);
			free(command);
		}
	}
	module_notice(bot, from->numericstr, "End of list.");
	return 0;
}
