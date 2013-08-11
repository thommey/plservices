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

extern time_t now;

static char chanmodelist[128];
static char usermodelist[128];
static char servermodelist[128];
static jtableL modebufs; /* struct from * -> struct modebuf * */

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

static void mode_flushmode_deletion(struct entity *from) {
	struct modebuf *m = jtableL_get(&modebufs, (unsigned long)from);
	if (m)
		mode_flushmode(m);
}

static void mode_flushmode_check(struct entity *from, struct modebuf *m, void *nothing) {
	if (!m->timeout)
		return;
	if (now >= m->timeout)
		mode_flushmode(m);
}

void mode_ontick(void) {
	jtableL_iterate(&modebufs, (jtableL_cb)mode_flushmode_check, NULL);
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
	hook_hook("ontick", mode_ontick);
	hook_hook("onuserdel", mode_flushmode_deletion);
	hook_hook("onserverdel", mode_flushmode_deletion);
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
	if (!m)
		return;

	/* targetstr and modestr aren't \0 terminated */
	if (m->modestrpos != m->modestr)
		send_format("%s M %s %.*s%.*s", verify_user(m->from) ? ((struct user *)m->from)->numericstr : ((struct server *)m->from)->numericstr, m->chan->name, m->modestrpos - m->modestr, m->modestr, m->targetstrpos - m->targetstr, m->targetstr);
	/* reset mode buffer */
	jtableL_remove(&modebufs, (unsigned long)m->from);
	free(m);
}

/* maxdelay = 0 means at next tick,
 * maxdelay = -1 means caller arranges for flushmode to be called
 */
/* TODO: store channel ts to detect channel dropping and recreation? */
struct modebuf *mode_pushmode(struct entity *from, struct channel *c, int plsmns, char mode, const char *target, size_t targetlen, int maxdelay) {
	struct modebuf *m;
	struct user *u;

	assert(verify_server(from) || verify_user(from));

	m = jtableL_get(&modebufs, (unsigned long)from);

	/* check general validity of modechar */
	if (!hasmode_valid(chanmodelist, mode)) {
		logfmt(LOG_WARNING, "Invalid mode change: %c'%c'", plsmns ? '+' : '-', mode);
		return m;
	}

	/* check mode parameter requirement */
	if (target == NULL && (plsmns ? hasmode_setparam(chanmodelist, mode) : hasmode_unsetparam(chanmodelist, mode))) {
		logfmt(LOG_WARNING, "Invalid mode change: %c'%c', needs parameter.", plsmns ? '+' : '-', mode);
		return m;
	}

	/* ignore modes from non-ops */
	if (verify_user(from) && (!chanusers_ison((struct user *)from, c) || !channel_isop(c, (struct user *)from)))
		return m;

	/* check target validity, ignore redundant modes */
	if (ismode_prefix(chanmodelist, mode)) {
		u = get_user_by_numericstr(target);
		if (!u || !chanusers_ison(u, c)) {
			logfmt(LOG_WARNING, "Mode change target user not found or not on channel: '%s' on '%s'", target, c->name);
			return m;
		}
		if ((mode == 'o' && plsmns == channel_isop(c, u)) ||
			(mode == 'v' && plsmns == channel_isvoice(c, u)))
			return m;
	} else if (plsmns == mode_check1(&c->mode, mode))
		return m;

	/* only aggregate from same user on same channel */
	if (m && m->chan != c) {
		mode_flushmode(m);
		m = NULL;
	}

	if (!m) {
		m = smalloc(sizeof(*m));
		m->from = from;
		m->chan = c;
		m->lastplsmns = -1;
		m->timeout = 0;
		m->modestrpos = m->modestr;
		m->targetstrpos = m->targetstr;
		m->modecount = 0;
		jtableL_insert(&modebufs, (unsigned long)from, m);
	}

	if (maxdelay == -1)
		m->timeout = 0;
	else if (!m->timeout || now + maxdelay < m->timeout)
		m->timeout = now + maxdelay;

	/* write mode char and opt. prefix */
	if (m->lastplsmns != plsmns) {
		*m->modestrpos++ = plsmns ? '+' : '-';
		m->lastplsmns = plsmns;
	}
	*m->modestrpos++ = mode;

	/* write target */
	if (target) {
		*m->targetstrpos++ = ' ';
		strncpy(m->targetstrpos, target, targetlen);
		m->targetstrpos += targetlen;
		m->modecount++; /* only count modes with target, that's where the limit of 6 is at */
	}

	if (m->modecount == 6) {
		mode_flushmode(m);
		return NULL;
	}

	return m;
}
