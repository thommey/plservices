#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "main.h"

//hook_hook("onprivmsg", luabase_onprivmsg);
void onprivmsg(struct user *from, struct user *to, char *msg);

int load() { 
	hook_hook("onprivmsg", onprivmsg);
	printf("Loaded.\n");
	return 0;
}

int unload() { 
	printf("Unloaded.\n");
	return 0;
}

void onprivmsg(struct user *from, struct user *to, char *msg) {
	printf("Privmsg: %s\n", msg);
}
