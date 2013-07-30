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

#ifndef CHANNELS_H_
#define CHANNELS_H_

#define MAGIC_CHANNEL  0x2ED4
#define MAGIC_CHANNEL0 0x38B7

#define CNAMELEN 200
#define KEYLEN 23
#define TOPICLEN 250

struct channel {
	unsigned int magic;
	char name[CNAMELEN+1];
	char key[KEYLEN+1];
	char topic[TOPICLEN+1];
	long limit;
	time_t ts;
	int usercount;
	chanmode mode;
	jtableP ops, voices, users;  /* o = opped users, v = voiced users, users = all users */
	jtableS bans;
};

/* mask origin (ban setter) */
struct maskfrom {
	char nick[NICKLEN+1];
	char user[USERLEN+1];
	char host[HOSTLEN+1];
	char account[ACCOUNTLEN+1];
};

/* bans etc */
struct mask {
	char mask[NICKLEN+1+USERLEN+1+HOSTLEN+1];
	struct maskfrom from;
	time_t ts;
};

#define verify_channel(e)  (((struct entity *)(e))->magic == MAGIC_CHANNEL)
#define verify_channel0(e) (((struct entity *)(e))->magic == MAGIC_CHANNEL0)

struct channel *get_channel_by_name(const char *name);
struct channel *add_channel(char *name, time_t ts);
void del_channel(struct channel *c);
void channel_op(struct channel *c, struct user *u);
int channel_isop(struct channel *c, struct user *u);
int channel_isvoice(struct channel *c, struct user *u);
int channel_isoporvoice(struct channel *c, struct user *u);
int channel_isregular(struct channel *c, struct user *u);
void channel_plsban(struct channel *c, struct entity *e, char *str);
void channel_mnsban(struct channel *c, struct entity *e, char *str);
void channel_plsprefix(struct channel *c, struct user *u, char mc);
void channel_mnsprefix(struct channel *c, struct user *u, char mc);
int channel_apply_mode(struct entity *from, struct channel *target, char *modechanges, struct manyargs *arg, int skip);
void channel_burstmode(struct channel *chan, struct user *user, char *modes);
int channel_apply_clearmode(struct entity *from, struct entity *target, char *modes);

#endif /* CHANNELS_H_ */
