#include <string.h>
#include <assert.h>

#include "main.h"

static jtable channels = (jtable)NULL;

struct channel *get_channel_by_name(char *name) {
	return jtableS_get(&channels, name);
}

struct channel *add_channel(char *name, time_t ts) {
	struct channel *c;

	c = zmalloc(sizeof(*c));
	c->magic = MAGIC_CHANNEL;
	strbufcpy(c->name, name);
	c->ops = (jtable)NULL;
	c->voices = (jtable)NULL;
	c->users = (jtable)NULL;
	c->bans = (jtable)NULL;
	c->ts = ts;

	return jtableS_insert(&channels, name, c);
}

void del_channel(struct channel *c) {
	jtableP_free(&c->users);
	jtableP_free(&c->ops);
	jtableP_free(&c->voices);
	jtableS_remove(&channels, c->name);
	free(c);
}

void channel_add_user(struct channel *c, struct user *u) {
	jtableP_set(&c->users, u);
}

void channel_del_user(struct channel *c, struct user *u) {
	jtableP_unset(&c->ops, u);
	jtableP_unset(&c->voices, u);
	jtableP_unset(&c->users, u);
}

void channel_plsprefix(struct channel *c, struct user *u, char mc) {
	assert(jtableP_check(&c->users, u));
	if (mc == 'o')
		jtableP_set(&c->ops, u);
	else if (mc == 'v')
		jtableP_set(&c->voices, u);
	else
		debug(LOG_WARNING, "Invalid prefix mode change: %c", mc);
}

void channel_mnsprefix(struct channel *c, struct user *u, char mc) {
	assert(jtableP_check(&c->users, u));
	if (mc == 'o')
		jtableP_unset(&c->ops, u);
	else if (mc == 'v')
		jtableP_unset(&c->voices, u);
	else
		debug(LOG_WARNING, "Invalid prefix mode change: %c", mc);
}

void channel_plsban(struct channel *c, struct entity *e, char *str) {
	struct mask *m;
	struct user *u;

	m = smalloc(sizeof(*m));
	if (verify_user(e)) {
		u = (struct user *)e;
		strbufcpy(m->from.nick, u->nick);
		strbufcpy(m->from.user, u->user);
		strbufcpy(m->from.host, u->host);
		strbufcpy(m->from.account, u->account);
	} else
		debug(LOG_WARNING, "Ban set by non-user.");
	strbufcpy(m->mask, str);
	m->ts = now;
	jtableS_insert(&c->bans, str, m);
}

void channel_mnsban(struct channel *c, struct entity *e, char *str) {
	struct mask *m;

	if (!verify_user(e))
		debug(LOG_WARNING, "Ban unset by non-user.");
	m = jtableS_get(&c->bans, str);
	if (!m) {
		debug(LOG_WARNING, "Non-existant ban removed.");
		return;
	}
	jtableS_remove(&c->bans, str);
	sfree(m);
}

int channel_isop(struct channel *c, struct user *u) {
	return jtableP_check(&c->ops, u);
}

int channel_isvoice(struct channel *c, struct user *u) {
	return jtableP_check(&c->ops, u);
}

int channel_isoporvoice(struct channel *c, struct user *u) {
	return jtableP_check(&c->ops, u) || jtableP_check(&c->voices, u);
}

int channel_isregular(struct channel *c, struct user *u) {
	return !channel_isoporvoice(c, u);
}

int channel_ison(struct channel *c, struct user *u) {
	return jtableP_check(&c->users, u);
}

int channel_modehook(struct entity *from, struct channel *c, int pls, char modechange, char *param) {
	struct user *u;
	long tmp;

	if (!verify_channel(c)) {
		debug(LOG_ERROR, "Mode change for not-a-channel!");
		return MODEHOOK_ERROR;
	}

	switch (modechange) {
	case 'k':
		if (!pls) {
			if (strcmp(c->key, param))
				debug(LOG_WARNING, "Unset key that wasn't set. Mine: '%s', theirs: '%s'.", c->key, param);
			c->key[0] = '\0';
		} else
			strbufcpy(c->key, param);
		break;
	case 'l':
		tmp = strtol(param, NULL, 10);
		if (pls && tmp <= 0) {
			debug(LOG_WARNING, "Invalid limit setting: '%s' (=> %ld)", param, tmp);
		}
		c->limit = tmp;
		break;
	case 'b':
		if (!param || !param[0]) {
			debug(LOG_WARNING, "Invalid ban mode change or non-existant ban.");
			return MODEHOOK_OK;
		}
		pls ? channel_plsban(c, from, param) : channel_mnsban(c, from, param);
	case 'o': /* fall-through */
	case 'v':
		u = get_user_by_numeric(param);
		if (!u || !verify_user(u)) {
			debug(LOG_WARNING, "Op/voice for non-existant/invalid user!");
			return MODEHOOK_OK;
		}
		pls ? channel_plsprefix(c, u, modechange) : channel_mnsprefix(c, u, modechange);
		break;
	}
	return MODEHOOK_OK;
}
