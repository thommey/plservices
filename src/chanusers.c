/**
 * chanusers.c - Manages relationship between channels and users in both ways
 *               (channels have userlist, users have channellist)
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
**/

#include "main.h"

/* the following static functions are implement here, because they shouldn't be
 * accessed by anything else, use chanusers_ison/join/leave instead. */

/* CHANNELS */

static int channel_ison(struct channel *c, struct user *u) {
	return jtableP_check(&c->users, u);
}

static void channel_add_user(struct channel *c, struct user *u) {
	c->usercount += jtableP_set(&c->users, u);
}

static void channel_del_user(struct channel *c, struct user *u) {
	jtableP_unset(&c->ops, u);
	jtableP_unset(&c->voices, u);
	c->usercount -= jtableP_unset(&c->users, u);
}

/* USERS */

static int user_ison(struct user *u, struct channel *c) {
	return jtableP_check(&u->channels, c);
}

static void user_add_channel(struct user *u, struct channel *c) {
	u->channelcount += jtableP_set(&u->channels, c);
}

static void user_del_channel(struct user *u, struct channel *c) {
	u->channelcount -= jtableP_unset(&u->channels, c);
}

/* visible functions */

void chanusers_del_channel(struct channel *c) {
	jtableP_iterate1(&c->users, (void (*)(void *, void *))user_del_channel, c);
}

void chanusers_del_user(struct user *u) {
	jtableP_iterate1(&u->channels, (void (*)(void *, void *))channel_del_user, u);
}

void chanusers_join(struct user *u, struct channel *c) {
	channel_add_user(c, u);
	user_add_channel(u, c);
}

void chanusers_leave(struct user *u, struct channel *c) {
	channel_del_user(c, u);
	user_del_channel(u, c);
}

int chanusers_ison(struct user *u, struct channel *c) {
	if (u->channelcount < c->usercount)
		return user_ison(u, c);
	else
		return channel_ison(c, u);
}

void chanusers_join0(struct user *u) {
	chanusers_del_user(u);
	jtableP_free(&u->channels);
	assert(u->channelcount == 0);
}
