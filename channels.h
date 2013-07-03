#ifndef CHANNELS_H_
#define CHANNELS_H_

#define MAGIC_CHANNEL 0x2ED4

#define CNAMELEN 200
#define KEYLEN 50

struct channel {
	unsigned int magic;
	char name[CNAMELEN+1];
	char key[KEYLEN+1];
	long limit;
	time_t ts;
	chanmode mode;
	jtable ops, voices, users;  /* o = opped users, v = voiced users, users = all users */
	jtable bans;
};

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

#define verify_channel(e) ((e)->magic == MAGIC_CHANNEL)

struct channel *get_channel_by_name(char *name);
struct channel *add_channel(char *name, time_t ts);
void del_channel(struct channel *c);
void channel_add_user(struct channel *c, struct user *u);
void channel_del_user(struct channel *c, struct user *u);
void channel_op(struct channel *c, struct user *u);
int channel_isop(struct channel *c, struct user *u);
int channel_isvoice(struct channel *c, struct user *u);
int channel_isoporvoice(struct channel *c, struct user *u);
int channel_isregular(struct channel *c, struct user *u);
int channel_ison(struct channel *c, struct user *u);

#endif // CHANNELS_H_
