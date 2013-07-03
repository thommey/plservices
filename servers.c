#include <string.h>

#include "main.h"

static jtable serverlist = (jtable)NULL;

struct server *get_server_by_numeric(char *numeric) {
	return jtableS_get(&serverlist, numeric);
}

struct server *add_server(char *numeric, char *name, int hops, time_t boot, time_t link, char *protocol, long maxusers, char *description) {
	struct server *s;

	s = zmalloc(sizeof(*s));
	s->magic = MAGIC_SERVER;
	strbufcpy(s->numeric, numeric);
	strbufcpy(s->name, name);
	s->hops = hops;
	s->boot = boot;
	s->link = link;
	strbufcpy(s->protocol, protocol);
	s->maxusers = maxusers;
	strbufcpy(s->description, description);

	return jtableS_insert(&serverlist, numeric, s);
}

void del_server(struct server *server) {
	jtableS_remove(&serverlist, server->numeric);
	free(server);
}

