#include "main.h"
#include "tclcontrol.h"
#include <string.h>
#include <strings.h>

int command_version (struct user *from, struct user *to, struct manyargs *args) {
	module_notice(bot, from->numericstr, "VERSION The TCL Control Service v%s by %s", MOD_VERSION, MOD_AUTHOR);
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
		if (commands[x].trigger != NULL && !strcasecmp(commands[x].trigger, args->v[1])) {
			module_notice(bot, from->numericstr, "Showing help for command %s", strconv(toupper, NULL, 0, args->v[1]));
			module_notice(bot, from->numericstr, "Usage: %s", commands[x].syntax);
			module_notice(bot, from->numericstr, "%s", commands[x].description);
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
			module_notice(bot, from->numericstr, "%s \t %s", strconv(toupper, NULL, 0, commands[x].trigger), commands[x].description);
		}
	}
	module_notice(bot, from->numericstr, "End of list.");
	return 0;
}
