#include "main.h"
#include "luabase.h"
#include "luacontrol.h"
#include <string.h>

int load() {
	bot = module_create_client(BOTNICK, BOTIDENT, BOTHOSTNAME, BOTMODES, BOTNICK, BOTNICK, BOTREALNAME);
	module_join_channel(bot, BOTDEBUGCHAN, 1);
	module_describe(bot, BOTDEBUGCHAN, "enters on a chariot of fire.");
	hook_hook("onprivmsg", onprivmsg);
	commands_initialize();
	logfmt(LOG_DEBUG, "(%s): Loaded.", MOD_NAME);
	return luabase_load();
}

int unload() {
	module_destroy_client(bot, "Module unloaded.");
	logfmt(LOG_DEBUG, "(%s): Unloaded.", MOD_NAME);
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
		if (commands[x].trigger != NULL && !strcmp(strtolower(commands[x].trigger), strtolower(trigger))) {
			return commands[x].cmdptr;
		}
	}
	return NULL;
}

void command_register(char *trigger, char *syntax, char *description, cmdfunc command) {
	int x = 0;
	for (; commands[x].trigger != NULL; x++);
	commands[x].trigger = trigger;
	commands[x].syntax = syntax;
	commands[x].description = description;
	commands[x].cmdptr = command;
}

void commands_initialize() {
	command_register("help", "HELP <command>", "Provides help such as syntaxes and descriptions for commands.", command_help);
	command_register("load", "LOAD <script>", "Loads a script file into the module.", command_load);
	command_register("showcommands", "SHOWCOMMANDS", "Shows the list of currently available commands.", command_showcommands);
	command_register("unload", "UNLOAD <script>", "Unloads a script file from the module.", command_unload);
	command_register("version", "VERSION", "Shows version information.", command_version);
}
