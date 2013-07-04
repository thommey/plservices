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
**/

#include "main.h"

/* Manages relationship between users and channels. Referenced in both directions (user has channellist, channel has userlist) */
/* only these should be called from outside, not channel_add_user/user_add_channel directly */

void chanusers_del_channel(struct channel *c) {
	jtableP_iterate1(&c->users, (void (*)(void *, void *))user_del_channel, c);
}

void chanusers_del_user(struct user *u) {
	jtableP_iterate1(&u->channels, (void (*)(void *, void *))channel_del_user, u);
}

void chanusers_join(struct channel *c, struct user *u) {
	channel_add_user(c, u);
	user_add_channel(u, c);
}

void chanusers_leave(struct channel *c, struct user *u) {
	channel_del_user(c, u);
	user_del_channel(u, c);
}
