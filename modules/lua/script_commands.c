#include "main.h"
#include "luabase.h"
#include "luacontrol.h"
#include <string.h>

int command_load (struct user *from, struct user *to, struct manyargs *args) {
        if (args->c == 1) {
		module_notice(bot, from->numericstr, "You didn't specify enough parameters for LOAD.");
		module_notice(bot, from->numericstr, "Usage: LOAD <script>");
		module_notice(bot, from->numericstr, "Loads a lua script file into the engine.");
		return 0;
	}
	char script[512];
	sprintf(script, "scripts/%s.lua", (char *)args->v[1]);
	if (!luabase_loadscript(script)) { 
		module_notice(bot, from->numericstr, "Done.");
	} else { 
		module_notice(bot, from->numericstr, "There was a problem loading '%s'.", args->v[1]);
		module_notice(bot, from->numericstr, "Please note that you do not need to include the '.lua' extension when loading a script.");
	}
        return 0;
}

int command_unload (struct user *from, struct user *to, struct manyargs *args) {
        if (args->c == 1) {
                module_notice(bot, from->numericstr, "You didn't specify enough parameters for UNLOAD.");
                module_notice(bot, from->numericstr, "Usage: UNLOAD <script>");
                module_notice(bot, from->numericstr, "Unloads a load script file.");
                return 0;
        }
        if (!luabase_valid_script(args->v[1])) {
                module_notice(bot, from->numericstr, "Unknown script %s.", args->v[1]);
                return 0;
        } else {
		char script[512];
		sprintf(script, "scripts/%s.lua", (char *)args->v[1]);
                if (!luabase_unloadscript(script)) {
                        module_notice(bot, from->numericstr, "Done.");
                } else { 
			module_notice(bot, from->numericstr, "There was a problem unloading '%s'.", args->v[1]);
			module_notice(bot, from->numericstr, "Please note that you do not need to include the '.lua' extension when unloading a script.");
		}
        }
        return 0;
}
