#include <time.h>
#include <string.h>
#include <assert.h>

#include "main.h"

#define VERIFY_SERVER(e) do { if (!e || !verify_server(e)) { debug(LOG_WARNING, "Server verification failed."); return; } } while (0);
#define VERIFY_USER(e) do { if (!e || !verify_user(e)) { debug(LOG_WARNING, "User verification failed."); return; } } while (0);
#define VERIFY_CHANNEL(e) do { if (!e || !verify_server(e)) { debug(LOG_WARNING, "Channel verification failed."); return; } } while (0);

void hACCOUNT(struct entity *from, struct user *user, char *accname) {
	VERIFY_USER(user);
	user_setaccount(user, accname);
}

void hADMIN(struct entity *from, struct server *server) {

}

void hASLL(struct entity *from, char *smark, struct server *target) {

}

void hAWAY(struct user *from, char *reason) {
	VERIFY_USER(from);
	user_setaway(from, reason);
}

void hBURST(struct entity *from, struct channel *chan, time_t *ts) {

}

void hCLEARMODE(struct entity *from, struct channel *chan, char *modes) {

}

void hCONNECT(struct entity *from, char *servername, int *port, struct server *server) {

}

void hCREATE(struct user *from, char *channels, time_t *ts) {
	int i;
	struct manyargs *chlist;
	struct channel *c;

	VERIFY_USER(from);

	chlist = commasplit(channels);
	for (i = 0; i < chlist->c; i++) {
		c = get_channel_by_name(chlist->v[i]);
		if (c) {
			debug(LOG_WARNING, "CREATE for existing channel: %s. Deleting.", chlist->v[i]);
			del_channel(c);
		}
		c = add_channel(chlist->v[i], *ts);
		channel_add_user(c, from);
		channel_plsprefix(c, from, 'o');
	}
}

void hDESTRUCT(struct entity *from, struct channel *chan, time_t *ts) {
	if (chan->ts != *ts)
		debug(LOG_WARNING, "DESTRUCT for channel with mismatched ts (mine: %ld, theirs: %ld)", chan->ts, *ts);
	del_channel(chan);
}

void hDESYNCH(struct entity *from, char *msg) {

}

void hEND_OF_BURST(struct server *from) {
	VERIFY_SERVER(from);
	assert(!strcmp(from->protocol, "J10"));
	from->protocol[0] = 'P';
	send_words(0, "AC", "EA");
}

void hEOB_ACK(struct entity *from) {
	// TODO set my proto to P10
}

void hERROR(struct entity *from, char *msg) {
	if (msg && msg[0])
		debug(LOG_FATAL, "Remote server gave me ERROR: %s", msg);
	error("Remote server gave ERROR.");
}

void hGLINE1(struct entity *from, char *mask, time_t *duration, char *reason) {
}

void hGLINE2(struct entity *from, char *mask, time_t *duration, time_t *lastmod, char *reason) {
}

void hINFO(struct entity *from, struct server *target) {
}

void hINVITE(struct entity *from, struct user *target, struct channel *chan) {
}

void hJOIN(struct user *from, struct channel *chan, time_t *ts) {
	VERIFY_USER(from);
	VERIFY_CHANNEL(chan);

	// TODO join 0?
	if (chan->ts != *ts)
		debug(LOG_WARNING, "JOIN with wrong channel ts.");
	channel_add_user(chan, from);
}

void hJUPE(struct entity *from, struct server *target, char *server, time_t *duration, time_t *lastmod, char *reason) {
}

void hKICK(struct entity *from, struct channel *chan, struct user *target, char *reason) {
	VERIFY_CHANNEL(chan);
	VERIFY_USER(target);

	channel_del_user(chan, target);
}

void hKILL(struct entity *from, struct user *target, char *reason) {
	VERIFY_USER(target);

	del_user(target);
}

void hLINKS(struct entity *from, struct server *target, char *servermask) {
}

void hLUSERS(struct entity *from, struct server *target) {
}

void hMODE1(struct entity *from, struct user *user, struct manyargs *modechange) {
}

void hMODE2(struct entity *from, struct channel *chan, struct manyargs *modechange, time_t *ts) {
}

void hMOTD(struct entity *from, struct server *target) {
}

void hNAMES(struct entity *from, struct server *server) {
}

void hNICK1(struct user *from, char *newnick, time_t *ts) {
	VERIFY_USER(from);
	// TODO nick collision check
	user_setnick(from, newnick);
}

void hNICK2(struct entity *from, char *nick, int *hops, time_t *ts, char *ident, char *host, struct manyargs *mode, struct ip *ip, char *unum, char *realname) {
	add_user(unum, *hops, nick, ident, host, realname);
}

void hNOTICE(struct entity *from, char *target, char *msg) {
}

void hOPMODE(struct entity *from, struct channel *chan, struct manyargs *mode, time_t *ts) {
}

void hPART(struct user *user, char *channels, char *reason) {
	struct manyargs *chlist;
	struct channel *c;
	int i;
	VERIFY_USER(user);

	chlist = commasplit(channels);
	for (i = 0; i < chlist->c; i++) {
		c = get_channel_by_name(chlist->v[i]);
		VERIFY_CHANNEL(c);
		channel_del_user(c, user);
	}
}

void hPASS(struct entity *from, char *pass) {
}

void hPING(struct server *from, char *time1, char *target, char *time2) {
	VERIFY_SERVER(from);
	send_format("AC Z AC %s %s %d %s", time1, time2, 0, time2);
}

void hPONG(struct entity *from, struct server *source, char *param) {
}

void hPRIVMSG(struct entity *from, char *target, char *msg) {
}

void hQUIT(struct user *from, char *reason) {
	VERIFY_USER(from);
	del_user(from);
}

void hRPING1(struct entity *from, char *smask, struct server *startserver, char *text) {
}

void hRPING2(struct entity *from, struct server *target, struct user *oper, time_t *start, long *startms) {
}

void hRPONG1(struct entity *from, char *startserver, struct user *oper, time_t *start, long *startms) {
}

void hRPONG2(struct entity *from, struct user *target, char *servername, long *ms, char *text) {
}

void hSERVER(struct entity *from, char *name, int *hops, time_t *boot, time_t *link, char *protocol, char *numericstr, char *flags, char *descr) {
	static int uplinked = 0;

	char snum[3];
	char *maxuserstr = numericstr+2;

	set_registered(1);
	snum[0] = numericstr[0];
	snum[1] = numericstr[1];
	snum[2] = '\0';

	add_server(snum, name, *hops, *boot, *link, protocol, -1, descr);
	if (!uplinked) {
		uplinked = 1;
		send_raw("AC EB");
	}
}

void hSETHOST(struct entity *from, struct user *target, char *ident, char *host) {
}

void hSETTIME(struct entity *from, time_t *ts, struct server *server) {
}

void hSILENCE(struct entity *from, char *target, char *mask) {
}

void hSQUIT(struct entity *from, char *target, time_t *link, char *reason) {
}

void hSTATS(struct entity *from, char *stats, struct server *server, char *params) {
}

void hTOPIC(struct entity *from, struct channel *chan, time_t *creation, time_t *change, char *topic) {
}

void hTRACE(struct entity *from, char *param, struct server *target) {
}

void hUPING(struct entity *from, char *smask, int *port, struct server *target, int *count) {
}

void hVERSION(struct entity *from, struct server *target) {
}

void hWALLCHOPS(struct entity *from, struct channel *chan, char *msg) {
}

void hWALLOPS(struct entity *from, char *msg) {
}

void hWALLUSERS(struct entity *from, char *msg) {
}

void hWALLVOICES(struct entity *from, struct channel *chan, char *msg) {
}

void hWHOIS(struct entity *from, struct server *server, char *search) {
}
