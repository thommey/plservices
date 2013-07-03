#include <string.h>
#include <assert.h>

#include "jtable.h"
#include "utils.h"
#include "users.h"
#include "channels.h"
#include "log.h"

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

void channel_op(struct channel *c, struct user *u) {
	assert(jtableP_check(&c->users, u));
	jtableP_set(&c->ops, u);
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
