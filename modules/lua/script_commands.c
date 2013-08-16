#include "main.h"
#include "luabase.h"
#include "luacontrol.h"
#include <string.h>

int command_load (struct user *from, struct user *to, struct manyargs args) {
        if (args.c == 1) { 
		module_notice(bot, from->numericstr, "You didn't specify enough parameters for LOAD.");
		module_notice(bot, from->numericstr, "Usage: LOAD <script>");
		module_notice(bot, from->numericstr, "Loads a lua script file into the engine.");
		return 0;
	}
	module_notice(bot, from->numericstr, "Args: %s - %d", args.v[1], args.c);
        return 0;
}

