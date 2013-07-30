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
 */

#ifndef SERVER_H_
#define SERVER_H_

#define MAGIC_SERVER 0xA839

#define SNAMELEN 63
#define DESCRLEN 150

#define ME (me->numeric)

struct server {
	unsigned int magic;
	unsigned long numeric;
	char name[SNAMELEN+1];
	char maxusers[4];
	int hops;
	time_t boot;
	time_t link;
	char protocol[4];
	char description[DESCRLEN+1];
	jtableP users;
	servermode mode;
};

#define verify_server(e) (((struct entity *)(e))->magic == MAGIC_SERVER)

struct server *get_server_by_numeric(unsigned long numeric);
struct server *get_server_by_numericstr(const char *numeric);
struct server *add_server(unsigned long numeric, char *maxusers, char *name, int hops, time_t boot, time_t link, char *protocol, char *description);
void del_server(struct server *user);
void server_apply_mode(struct entity *from, struct entity *target, char *modechanges, struct manyargs *arg, int skip);

#endif /* SERVER_H_ */
