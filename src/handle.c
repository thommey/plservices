/**
 * handle.c - Handling routines for all P10 tokens
 *
 * Copyright (c) 2013 Thomas Sader (thommey)
 *
 *  This file is part of PLservices.
 *
 *  PLservices is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PLservices is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PLservices.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */

#include <time.h>
#include <string.h>
#include <assert.h>

#include "main.h"

#define VERIFY_SERVER(e) do { if (!e || !verify_server(e)) { logtxt(LOG_WARNING, "Server verification failed."); return; } } while (0);
#define VERIFY_USER(e) do { if (!e || !verify_user(e)) { logtxt(LOG_WARNING, "User verification failed."); return; } } while (0);
#define VERIFY_CHANNEL(e) do { if (!e || !verify_channel(e)) { logtxt(LOG_WARNING, "Channel verification failed."); return; } } while (0);

extern struct server *me, *uplink;

static int end_of_burst = 0;

void hACCOUNT(struct entity *from, struct user *user, char *accname, time_t *ts, long *accid, long *flags) {
	VERIFY_USER(user);
	user_setaccount(user, accname, ts ? *ts : 0, accid ? *accid : 0, flags ? *flags : 0);
}

void hADMIN(struct entity *from, struct server *server) {

}

void hASLL(struct entity *from, char *smark, struct server *target) {

}

void hAWAY(struct user *from, char *reason) {
	VERIFY_USER(from);
	user_setawaymsg(from, reason);
}

void hBURST(struct server *from, char *chan, time_t *ts, struct manyargs *rest) {
	struct manyargs list;
	struct channel *c;
	struct user *u;
	char *tmp, *burstmode = "";
	int nextpos, i;

	VERIFY_SERVER(from);

	if (from->protocol[0] == 'P') {
		logtxt(LOG_ERROR, "BURST after END_OF_BURST!");
		return;
	}
	c = get_channel_by_name(chan);

	if (!c)
		c = add_channel(chan, *ts);

	if (!rest->c)
		return;

	/* handle modes if present */
	nextpos = 0;

	tmp = rest->v[nextpos];
	if (tmp && tmp[0] == '+')
		nextpos = 1 + channel_apply_mode((struct entity *)from, c, rest->v[0], rest, 1);

	/* rest->v[nextpos] is now the next parameter after the modes */
	/* check the last parameter for leading % */
	tmp = rest->v[nextpos];
	if (rest->c - 1 > nextpos)
		tmp = rest->v[rest->c - 1];
	if (tmp && tmp[0] == '%') {
		split(&list, rest->v[rest->c - 1] + 1, ' ');
		for (i = 0; i < list.c; i++)
			channel_plsban(c, NULL, list.v[i]);
		rest->c--;
	}
	/* no channel users */
	if (!tmp)
		return;

	assert(nextpos+1 == rest->c);
	split(&list, rest->v[nextpos], ',');
	/* all that's left from rest->v[i .. rest->c] are user entries with tmps */
	for (i = 0; i < list.c; i++) {
		tmp = list.v[i];
		if ((tmp = strchr(tmp, ':'))) {
			*tmp++ = '\0';
			burstmode = tmp;
		}

		u = get_user_by_numericstr(list.v[i]);
		if (!u) {
			logtxt(LOG_WARNING, "Burst join for non-existant user.");
			continue;
		}
		chanusers_join(u, c);
		channel_burstmode(c, u, burstmode);
	}
}

void hCLEARMODE(struct entity *from, struct channel *chan, char *modes) {
	channel_apply_clearmode(from, (struct entity *)chan, modes);
}

void hCONNECT(struct entity *from, char *servername, int *port, struct server *server) {

}

void hCREATE(struct user *from, char *channels, time_t *ts) {
	int i;
	struct manyargs chlist;
	struct channel *c;

	VERIFY_USER(from);

	split(&chlist, channels, ',');
	for (i = 0; i < chlist.c; i++) {
		c = get_channel_by_name(chlist.v[i]);
		if (c) {
			logfmt(LOG_WARNING, "CREATE for existing channel: %s. Deleting.", chlist.v[i]);
			del_channel(c);
		}
		c = add_channel(chlist.v[i], *ts);
		chanusers_join(from, c);
		channel_plsprefix(c, from, 'o');
	}
}

void hDESTRUCT(struct entity *from, struct channel *chan, time_t *ts) {
	VERIFY_CHANNEL(chan);
	if (chan->ts != *ts)
		logfmt(LOG_WARNING, "DESTRUCT for channel with mismatched ts (mine: %ld, theirs: %ld)", chan->ts, *ts);
	del_channel(chan);
}

void hDESYNCH(struct entity *from, char *msg) {

}

void hEND_OF_BURST(struct server *from) {
	VERIFY_SERVER(from);
	if (from == me)
		return;
	assert(!strcmp(from->protocol, "J10"));
	from->protocol[0] = 'P';
	if (from == uplink) {
		end_of_burst = 1;
		server_send(me, "EB");
		server_send(me, "EA");
	}
}

void hEOB_ACK(struct server *from) {
	if (from == me)
		me->protocol[0] = 'P';
	else if (from == uplink)
		hook_call("onregistered", pack_empty());
}

void hERROR(struct entity *from, char *msg) {
	if (msg && msg[0])
		logfmt(LOG_FATAL, "Remote server gave me ERROR: %s", msg);
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

	if (verify_channel0(chan)) {
		chanusers_join0(from);
		return;
	}

	VERIFY_CHANNEL(chan);

	if (!ts)
		ts = &chan->ts;
	else if (chan->ts != *ts)
		logtxt(LOG_WARNING, "JOIN with wrong channel ts.");
	chanusers_join(from, chan);
}

void hJUPE(struct entity *from, struct server *target, char *server, time_t *duration, time_t *lastmod, char *reason) {
}

void hKICK(struct entity *from, struct channel *chan, struct user *target, char *reason) {
	VERIFY_CHANNEL(chan);
	VERIFY_USER(target);

	chanusers_leave(target, chan);
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
	VERIFY_USER(from);
	VERIFY_USER(user);
	user_apply_mode(from, user, modechange->v[0], modechange, 1);
}

void hMODE2(struct entity *from, struct channel *chan, struct manyargs *modechange, time_t *ts) {
	VERIFY_CHANNEL(chan);
	/* remote has older channel */
	if (ts && *ts < chan->ts)
		chan->ts = *ts;
	channel_apply_mode(from, chan, modechange->v[0], modechange, 1);
}

void hMOTD(struct entity *from, struct server *target) {
}

void hNAMES(struct entity *from, struct server *server) {
}

void hNICK1(struct user *from, char *newnick, time_t *ts) {
	VERIFY_USER(from);

	user_nickchange(from, newnick);
}

void hNICK2(struct entity *from, char *nick, int *hops, time_t *ts, char *ident, char *host, struct manyargs *mode, char *ip, char *unum, char *realname) {
	struct user *u;

	u = add_user(unum, *hops, nick, ident, host, realname);
	user_apply_mode((struct entity *)u, u, mode->v[0], mode, 1);
}

void hNOTICE(struct entity *from, char *target, char *msg) {
	struct user *u = (struct user *)from;

	if (!verify_user(u))
		return;
	if (u->server == me)
		return;
	if (*target != '#')
		hook_call("onprivnotc", pack_args(arg_user(u), arg_user(get_user_by_numeric(str2unum(target))), arg_str(msg)));
}

void hOPMODE(struct entity *from, struct channel *chan, struct manyargs *modechange, time_t *ts) {
	VERIFY_CHANNEL(chan);
	channel_apply_mode(from, chan, modechange->v[0], modechange, 1);
}

void hPART(struct user *user, char *channels, char *reason) {
	struct manyargs chlist;
	struct channel *c;
	int i;
	VERIFY_USER(user);

	split(&chlist, channels, ',');
	for (i = 0; i < chlist.c; i++) {
		c = get_channel_by_name(chlist.v[i]);
		VERIFY_CHANNEL(c);
		hook_call("onpart", pack_args(arg_user(user), arg_chan(c), arg_str(reason ? reason : "")));
		chanusers_leave(user, c);
	}
}

void hPASS(struct entity *from, char *pass) {
}

void hPING(struct server *from, struct manyargs *arg) {
	VERIFY_SERVER(from);
	send_format("%s Z %s %s %s %d %s", ME, ME, arg->v[0], arg->v[2], 0, arg->v[2]);
}

void hPONG(struct entity *from, struct server *source, struct manyargs *arg) {
}

void hPRIVMSG(struct entity *from, char *target, char *msg) {
	struct user *u = (struct user *)from;

	if (!verify_user(u))
		return;
	if (u->server == me)
		return;

	if (*target == '#')
		hook_call("onchanmsg", pack_args(arg_user(u), arg_chan(get_channel_by_name(target)), arg_str(msg)));
	else
		hook_call("onprivmsg", pack_args(arg_user(u), arg_user(get_user_by_numeric(str2unum(target))), arg_str(msg)));
}

void hQUIT(struct user *from, char *reason) {
	VERIFY_USER(from);
	hook_call("onquit", pack_args(arg_user(from), arg_str(reason ? reason : "")));
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

void hSERVER(struct entity *from, char *name, int *hops, time_t *boot, time_t *link, char *protocol, char *numeric, char *flags, char *descr) {
	struct server *s;
	char *maxuserstr = numeric+2;

	s = add_server(numeric, maxuserstr, name, *hops, *boot, *link, protocol, descr);

	server_apply_mode(from, (struct entity *)s, flags, NULL, 0);
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
