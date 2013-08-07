/**
 * modes.c - Mode handling and mode definitions as bitfields (all [a-zA-Z0-9])
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

static char chanmodelist[128];
static char usermodelist[128];
static char servermodelist[128];

#define mode2offset(m) (mode2offset_table[(unsigned char)(m)])

static int8_t mode2offset_table[128] = {
	/*  0 - 31 */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/* 32 - 63 */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, /* 0 - 9 */
	/* 64 - 95 */
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, /* A - O */
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, /* P - Z */
	/* 96 - 128 */
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* a - o */
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, /* p - z */
};

static struct {
	char *modes;
	char type;
} chanmodetable[] = {
	{ "b", MODE_BAN },
	{ "k", MODE_KEY },
	{ "l", MODE_LIMIT },
	{ "imnpstrDducCNMT", MODE_FLAG },
	{ "ov", MODE_PREFIX },
	{ NULL, 0 }
}, usermodetable[] = {
	{ "ohr", MODE_LIMIT },
	{ "wOidkXnIgxRPqQ", MODE_FLAG },
	{ NULL, 0 }
}, servermodetable[] = {
	{ "hsn6", MODE_FLAG },
	{ NULL, 0 }
};

static unsigned char getmodeflags(char *modelist, char modechar) {
	return modelist[(unsigned char)modechar];
}

static void registermode(char *modelist, char modechar, char flags) {
	modelist[(unsigned char)modechar] = flags;
}

void init_modes() {
	int i, j;

	for (i = 0; chanmodetable[i].modes; i++)
		for (j = 0; chanmodetable[i].modes[j]; j++)
			registermode(chanmodelist, chanmodetable[i].modes[j], chanmodetable[i].type);

	for (i = 0; usermodetable[i].modes; i++)
		for (j = 0; usermodetable[i].modes[j]; j++)
			registermode(usermodelist, usermodetable[i].modes[j], usermodetable[i].type);

	for (i = 0; servermodetable[i].modes; i++)
		for (j = 0; servermodetable[i].modes[j]; j++)
			registermode(servermodelist, servermodetable[i].modes[j], servermodetable[i].type);
}

void uplink_with_opername(void) {
	registermode(servermodelist, 'o', MODE_LIMIT);
}
void uplink_without_opername(void) {
	registermode(servermodelist, 'o', MODE_FLAG);
}

void mode_set1(uint64_t *modes, char modechar) {
	*modes |= ((uint64_t)1 << mode2offset(modechar));
}

int mode_check1(uint64_t *modes, char modechar) {
	return (*modes & ((uint64_t)1 << mode2offset(modechar))) ? 1 : 0;
}

void mode_unset1(uint64_t *modes, char modechar) {
	*modes &= ~((uint64_t)1 << mode2offset(modechar));
}

int mode_apply(struct entity *from, struct entity *target, uint64_t *modes, char *modechanges, struct manyargs *arg, int skip, modehook func) {
	char *modelist;
	char *mc = modechanges;
	char *param;
	int ret, argsused = 0, pls = 1;

	if (verify_channel(target))
		modelist = chanmodelist;
	else if (verify_user(target))
		modelist = usermodelist;
	else if (verify_server(target))
		modelist = servermodelist;
	else {
		logtxt(LOG_ERROR, "Mode change for unknown entity.");
		return 0;
	}

	while (*mc) {
		param = NULL;
		switch (*mc) {
		case '+':
			pls = 1;
			break;
		case '-':
			pls = 0;
			break;
		default:
			if (!hasmode_valid(modelist, *mc)) {
				logfmt(LOG_WARNING, "Invalid mode char: %c in sequence %s!", *mc, modechanges);
				break;
			}
			if (pls) {
				if (!hasmode_list(modelist, *mc) && !hasmode_usermode(modelist, *mc))
					mode_set1(modes, *mc);
				if (hasmode_setparam(modelist, *mc)) {
					param = skip < arg->c ? arg->v[skip++] : NULL;
					argsused++;
				}
			} else {
				if (!hasmode_list(modelist, *mc) && !hasmode_usermode(modelist, *mc))
					mode_unset1(modes, *mc);
				if (hasmode_unsetparam(modelist, *mc)) {
					param = skip < arg->c ? arg->v[skip++] : NULL;
					argsused++;
				}
			}
			ret = func(from, target, pls, *mc, param);
			if (ret == MODEHOOK_OK)
				break;
			if (param && (ret == MODEHOOK_NOPARAM || ret == MODEHOOK_IGNORE))
				skip--, argsused--;
			if (ret == MODEHOOK_IGNORE)
				pls ? mode_unset1(modes, *mc) : mode_set1(modes, *mc);
			break;
		}
		mc++;
	}
	return argsused;
}

void mode_flushmode(struct modebuf *m) {
	if (!m || !m->modecount)
		return;

	/* targetstr and modestr aren't \0 terminated */
	send_format("%s M %s %.*s%.*s", m->from->numericstr, m->chan->name, m->modestrpos - m->modestr, m->modestr, m->targetstrpos - m->targetstr, m->targetstr);
	/* reset mode buffer */
	m->from = NULL;
	m->chan = NULL;
	m->modecount = 0;
	m->lastplsmns = -1;
	m->modestrpos = m->modestr;
	m->targetstrpos = m->targetstr;
}

/* TODO: modebuffer per channel? per origin user? with timer to flush? */
struct modebuf *mode_pushmode(struct user *from, struct channel *c, int plsmns, char mode, const char *target, size_t targetlen) {
	static struct modebuf m = {NULL, NULL, 0, -1, NULL, "", NULL, ""};
	struct user *u;

	if (!m.modestrpos)
		m.modestrpos = m.modestr;
	if (!m.targetstrpos)
		m.targetstrpos = m.targetstr;

	/* check general validity of modechar */
	if (!hasmode_valid(chanmodelist, mode)) {
		logfmt(LOG_WARNING, "Invalid mode change: %c'%c'", plsmns ? '+' : '-', mode);
		return &m;
	}

	/* check mode parameter requirement */
	if (target == NULL && (plsmns ? hasmode_setparam(chanmodelist, mode) : hasmode_unsetparam(chanmodelist, mode))) {
		logfmt(LOG_WARNING, "Invalid mode change: %c'%c', needs parameter.", plsmns ? '+' : '-', mode);
		return &m;
	}

	/* ignore modes from non-ops */
	if (!chanusers_ison(from, c) || !channel_isop(c, from))
		return &m;

	/* only aggregate from same user on same channel */
	if (m.from != from || m.chan != c) {
		mode_flushmode(&m);
		m.from = from;
		m.chan = c;
	}

	/* check target validity, ignore redundant modes */
	if (ismode_prefix(chanmodelist, mode)) {
		u = get_user_by_numericstr(target);
		if (!u || !chanusers_ison(u, c)) {
			logfmt(LOG_WARNING, "Mode change target user not found or not on channel: '%s' on '%s'", target, c->name);
			return &m;
		}
		if ((mode == 'o' && plsmns == channel_isop(c, u)) ||
			(mode == 'v' && plsmns == channel_isvoice(c, u)))
			return &m;
	} else if (plsmns == mode_check1(&c->mode, mode))
		return &m;

	/* write mode char and opt. prefix */
	if (m.lastplsmns != plsmns) {
		*m.modestrpos++ = plsmns ? '+' : '-';
		m.lastplsmns = plsmns;
	}
	*m.modestrpos++ = mode;

	/* write target */
	if (target) {
		*m.targetstrpos++ = ' ';
		strncpy(m.targetstrpos, target, targetlen);
		m.targetstrpos += targetlen;
		m.modecount++; /* only count modes with target, that's where the limit of 6 is at */
	}

	if (m.modecount == 6)
		mode_flushmode(&m);

	return &m;
}
