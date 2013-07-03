#ifndef SERVER_H_
#define SERVER_H_

#include <time.h>

#define MAGIC_SERVER 0xA839

#define SNUMLEN 2
#define SNAMELEN 63
#define DESCRLEN 150

struct server {
	unsigned int magic;
	char numeric[SNUMLEN+1];
	char name[SNAMELEN+1];
	int hops;
	time_t boot;
	time_t link;
	char protocol[4];
	long maxusers;
	char description[DESCRLEN+1];
};

#define verify_server(e) ((e)->magic == MAGIC_SERVER)

struct server *get_server_by_numeric(char *numeric);
struct server *add_server(char *numeric, char *name, int hops, time_t boot, time_t link, char *protocol, long maxusers, char *description);
void del_server(struct server *user);

#endif // SERVER_H_
