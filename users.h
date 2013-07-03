#ifndef USER_H_
#define USER_H_

#define UNUMLEN 5
#define NICKLEN 15
#define USERLEN 9
#define HOSTLEN 63
#define REALNAMELEN 150
#define USERMODELEN 20
#define AWAYMSGLEN 150

#define MAGIC_USER 0x49F1

struct user {
	unsigned int magic;
	char numeric[UNUMLEN+1];
	char nick[NICKLEN+1];
	char user[USERLEN+1];
	char host[HOSTLEN+1];
	char account[NICKLEN+1];
	char realname[REALNAMELEN+1];
	int hops;
	unsigned int accountid;
	char awaymsg[AWAYMSGLEN+1];
	char umode[USERMODELEN+1];
	char fakeuser[USERLEN+1];
	char fakehost[HOSTLEN+1];
};

#define verify_user(e) ((e)->magic == MAGIC_USER)

struct user *get_user_by_numeric(char *numeric);
struct user *add_user(char *numeric, int hops, const char *nick, const char *user, const char *host, const char *realname);
void del_user(struct user *user);
void user_setaccount(struct user *u, char *accname);
void user_setaway(struct user *u, char *msg);
void user_setnick(struct user *u, char *newnick);

#endif // USER_H_
