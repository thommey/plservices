/**
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

#ifndef USER_H_
#define USER_H_

#include <time.h>

#define NICKLEN 15
#define USERLEN 10
#define HOSTLEN 63
#define ACCOUNTLEN NICKLEN
#define REALNAMELEN 50
#define USERMODELEN 20
#define AWAYMSGLEN 160

#define MAGIC_USER 0x49F1

struct user {
	unsigned int magic;
	unsigned long numeric;
	char numericstr[UNUMLEN+1];
	char nick[NICKLEN+1];
	char user[USERLEN+1];
	char host[HOSTLEN+1];
	char fakehost[USERLEN+1+HOSTLEN+1];
	char opername[NICKLEN+1];
	char account[NICKLEN+1];
	time_t acctime;
	long accid, accflags;
	char realname[REALNAMELEN+1];
	int hops;
	unsigned int accountid;
	char awaymsg[AWAYMSGLEN+1];
	usermode mode;
	int channelcount;
	jtableP channels;
	struct server *server;
};

#define verify_user(e) (((struct entity *)(e))->magic == MAGIC_USER)
#define user_send(u, ...) send_words(unum2str(u->numeric), __VA_ARGS__)

struct channel;

struct user *get_user_by_numeric(unsigned long numeric);
struct user *get_user_by_numericstr(const char *numeric);
struct user *get_user_by_nick(const char *nick);
struct user *add_user(char *numeric, int hops, char *nick, const char *user, const char *host, const char *realname);
void del_user_iter(void *uptr, void *param);
void del_user(struct user *user);
void user_apply_mode(struct entity *from, struct user *target, char *modechanges, struct manyargs *arg, int skip);
void user_nickchange(struct user *u, char *newnick);
int user_isoper(struct user *u);

void user_setaccount(struct user *u, char *accname, time_t ts, long id, long flags);
void user_unsetaccount(struct user *u);
#define ADDBUFPROTO(name) void user_set ## name (struct user *u, char *str); void user_unset ## name (struct user *u);
ADDBUFPROTO(nick)
ADDBUFPROTO(opername)
ADDBUFPROTO(fakehost)
ADDBUFPROTO(awaymsg)
#undef ADDBUFPROTO


#endif /* USER_H_ */
