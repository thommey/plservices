/**
 * channels.c - Channel management, including channel modes
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

#include <string.h>
#include <assert.h>

#include "main.h"

extern time_t now;

struct channel chan0 = { MAGIC_CHANNEL0 };

/* jtableS of channels, key = channel name, value = struct channel ptr */
static jtableS channels;

/* channel list management */
struct channel *add_channel(char *name, time_t ts) {
	struct channel *c;

	c = zmalloc(sizeof(*c));
	c->magic = MAGIC_CHANNEL;
	strbufcpy(c->name, name);
	c->ts = ts;

	return jtableS_insert(&channels, strconv(rfc_tolower, NULL, 0, name), c);
}

void del_channel(struct channel *c) {
	hook_call("onchanneldel", pack_args(arg_ptr(c)));
	chanusers_del_channel(c);
	jtableP_free(&c->users);
	jtableP_free(&c->ops);
	jtableP_free(&c->voices);
	jtableS_remove(&channels, c->name);
	free(c);
}

struct channel *get_channel(struct channel *c) {
	if (!c || !verify_channel(c))
		return NULL;
	return c;
};

struct channel *get_channel_by_name(const char *name) {
	return jtableS_get(&channels, strconv(rfc_tolower, NULL, 0, name));
}

void channel_plsprefix(struct channel *c, struct user *u, char mc) {
	assert(chanusers_ison(u, c));
	if (mc == 'o')
		jtableP_set(&c->ops, u);
	else if (mc == 'v')
		jtableP_set(&c->voices, u);
	else
		logfmt(LOG_WARNING, "Invalid prefix mode change: %c", mc);
}

void channel_mnsprefix(struct channel *c, struct user *u, char mc) {
	assert(chanusers_ison(u, c));
	if (mc == 'o')
		jtableP_unset(&c->ops, u);
	else if (mc == 'v')
		jtableP_unset(&c->voices, u);
	else
		logfmt(LOG_WARNING, "Invalid prefix mode change: %c", mc);
}

void channel_plsban(struct channel *c, struct entity *e, char *str) {
	struct mask *m;
	struct user *u;

	m = zmalloc(sizeof(*m));
	if (e && verify_user(e)) {
		u = (struct user *)e;
		strbufcpy(m->from.nick, u->nick);
		strbufcpy(m->from.user, u->user);
		strbufcpy(m->from.host, u->host);
		strbufcpy(m->from.account, u->account);
	}
	strbufcpy(m->mask, str);
	m->ts = now;
	jtableS_insert(&c->bans, str, m);
}

void channel_mnsban(struct channel *c, struct entity *e, char *str) {
	struct mask *m;

	if (!verify_user(e))
		logtxt(LOG_WARNING, "Ban unset by non-user.");
	m = jtableS_get(&c->bans, str);
	if (!m) {
		logtxt(LOG_WARNING, "Non-existant ban removed.");
		return;
	}
	jtableS_remove(&c->bans, str);
	sfree(m);
}

int channel_isop(struct channel *c, struct user *u) {
	return jtableP_check(&c->ops, u);
}

int channel_isvoice(struct channel *c, struct user *u) {
	return jtableP_check(&c->voices, u);
}

int channel_isoporvoice(struct channel *c, struct user *u) {
	return channel_isop(c, u) || channel_isvoice(c, u);
}

int channel_isregular(struct channel *c, struct user *u) {
	return !channel_isoporvoice(c, u);
}

/* deal with anything that's not just a trivial flag mode */
static int channel_modehook(struct entity *from, struct entity *target, int pls, char modechange, char *param) {
	struct channel *c;
	struct user *u;
	long tmp;

	c = (struct channel *)target;

	if (!verify_channel(c)) {
		logtxt(LOG_ERROR, "Mode change for not-a-channel!");
		return MODEHOOK_ERROR;
	}

	switch (modechange) {
		case 'k':
			if (!pls) {
				if (strcmp(c->key, param))
					logfmt(LOG_WARNING, "Unset key that wasn't set. Mine: '%s', theirs: '%s'.", c->key, param);
				c->key[0] = '\0';
			} else
				strbufcpy(c->key, param);
			break;
		case 'l':
			tmp = strtol(param, NULL, 10);
			if (pls && tmp <= 0) {
				logfmt(LOG_WARNING, "Invalid limit setting: '%s' (=> %ld)", param, tmp);
			}
			c->limit = tmp;
			break;
		case 'b':
			if (!param || !param[0]) {
				logtxt(LOG_WARNING, "Invalid ban mode change or non-existant ban.");
				return MODEHOOK_OK;
			}
			pls ? channel_plsban(c, from, param) : channel_mnsban(c, from, param);
		case 'o': /* fall-through */
		case 'v':
			u = get_user_by_numericstr(param);
			if (!u || !verify_user(u)) {
				logtxt(LOG_WARNING, "Op/voice for non-existant/invalid user!");
				return MODEHOOK_OK;
			}
			pls ? channel_plsprefix(c, u, modechange) : channel_mnsprefix(c, u, modechange);
			break;
	}
	return MODEHOOK_OK;
}

/* helper function to quickly set prefix modes during burst */
void channel_burstmode(struct channel *chan, struct user *user, char *modes) {
	while (*modes && *modes != ' ')
		channel_plsprefix(chan, user, *modes++);
}

/* function to apply channel modes from the outside */
int channel_apply_mode(struct entity *from, struct channel *target, char *modechanges, struct manyargs *arg, int skip) {
	if (!verify_channel(target)) {
		logtxt(LOG_WARNING, "Mode change on non-channel");
		return 0;
	}
	return mode_apply(from, (struct entity *)target, &target->mode, modechanges, arg, skip, channel_modehook);
}

/* clearmode shortcut (no iterating over users ..) */
int channel_apply_clearmode(struct entity *from, struct entity *target, char *modes) {
	struct channel *channel = (struct channel *)target;

	if (!verify_channel(channel)) {
		logtxt(LOG_WARNING, "Clearmode on non-channel");
		return 0;
	}
	while (*modes) {
		switch (*modes) {
			case 'o':
				jtableP_free(&channel->ops);
				break;
			case 'v':
				jtableP_free(&channel->voices);
				break;
			case 'b':
				jtableS_free(&channel->bans);
				break;
			case 'l':
				channel->limit = 0;
				mode_unset1(&channel->mode, *modes);
				break;
			case 'k':
				channel->key[0] = 0;
				mode_unset1(&channel->mode, *modes);
				break;
			default:
				mode_unset1(&channel->mode, *modes);
		}
		modes++;
	}
	return 0;
}


/* DEBUG STUFF */
static void debug_print_chanuser(struct user *u, void *null) {
	logfmt(LOG_DEBUG, "    '%s!%s@%s' (%s)", u->nick, u->user, u->host, u->numeric);
}

static void debug_print_ban(const char *mask, struct mask *b, void *null) {
	logfmt(LOG_DEBUG, "    '%s' == '%s' (from %s @ %ld)", mask, b->mask, b->from.nick[0] ? b->from.nick : "/unknown/", b->ts);
}

static void debug_print_channel(const char *name, struct channel *c) {
	static char modebuf[64], *t;
	char i;

	if (!verify_channel(c)) {
		logtxt(LOG_DEBUG, "INVALID CHANNEL");
		return;
	}
	logfmt(LOG_DEBUG, "Channel info for '%s' == '%s'", name, c->name);
	logfmt(LOG_DEBUG, "  Key: '%s'. Limit: %ld. Creation: %ld", c->key, c->limit, c->ts);
	t = modebuf;
	for (i = 'a'; i <= 'z'; i++)
		if (mode_check1(&c->mode, i))
			*t++ = i;
	for (i = 'A'; i <= 'Z'; i++)
		if (mode_check1(&c->mode, i))
			*t++ = i;
	*t++ = '\0';
	logfmt(LOG_DEBUG, "  Mode: +%s", modebuf);
	logtxt(LOG_DEBUG, "  Bans:");
	jtableS_iterate(&c->bans, (jtableS_cb)debug_print_ban, NULL);
	logtxt(LOG_DEBUG, "  Users:");
	jtableP_iterate(&c->users, (jtableP_cb)debug_print_chanuser, NULL);
	logtxt(LOG_DEBUG, "  Ops:");
	jtableP_iterate(&c->ops, (jtableP_cb)debug_print_chanuser, NULL);
	logtxt(LOG_DEBUG, "  Voices:");
	jtableP_iterate(&c->voices, (jtableP_cb)debug_print_chanuser, NULL);
}

void debug_print_users(void) {
	logtxt(LOG_DEBUG, "------------- Channel -------------");
	jtableS_iterate(&channels, (jtableS_cb)debug_print_channel, NULL);
}
