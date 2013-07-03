#include <string.h>

#include "utils.h"
#include "jtable.h"
#include "users.h"
#include "log.h"

static jtable userlist = (jtable)NULL;
static jtable opers = (jtable)NULL;

struct user *get_user_by_numeric(char *numeric) {
	return jtableS_get(&userlist, numeric);
}

struct user *add_user(char *numeric, int hops, const char *nick, const char *user, const char *host, const char *realname) {
	struct user *u;

	u = zmalloc(sizeof(*u));
	u->magic = MAGIC_USER;
	strbufcpy(u->numeric, numeric);
	u->hops = hops;
	strbufcpy(u->nick, nick);
	strbufcpy(u->user, user);
	strbufcpy(u->host, host);
	strbufcpy(u->realname, realname);

	return jtableS_insert(&userlist, numeric, u);
}

void del_user(struct user *user) {
	jtableS_remove(&opers, user->numeric);
	jtableS_remove(&userlist, user->numeric);
	free(user);
}

void user_setaway(struct user *u, char *msg) {
	if (msg && msg[0])
		strbufcpy(u->awaymsg, msg);
	else
		u->awaymsg[0] = '\0';
}

void user_setaccount(struct user *u, char *accname) {
	strbufcpy(u->account, accname);
}

void user_setnick(struct user *u, char *newnick) {
	strbufcpy(u->nick, newnick);
}
