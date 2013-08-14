/**
 * net.c - tcp connection to server
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"

#undef send_format
#undef send_words
#undef send_raw
#undef logtxt
#undef logfmt

static int conn;

extern int now;
extern struct server *me;
extern char *base64chars;

static void net_greet(const char *pass, const char *sname, const char *sdescr);

void net_connect(void) {
	const char *uplink = config_get("core.uplink", "host");
	const char *port = config_get("core.uplink", "port");
	const char *pass = config_get("core.uplink", "pass");
	const char *numeric = config_get("core.server", "numeric");
	const char *servername= config_get("core.server", "name");
	const char *descr = config_get("core.server", "description");
	struct sockaddr_in server;
	int flag_set = 1;

	if (!uplink || !port || !pass)
		error("Invalid uplink specified in configfile. Need [core.uplink] host, port, pass");

	if (!servername || !descr || !numeric || strspn(numeric, base64chars) != 2)
		error("Invalid uplink specified in configfile. Need [core.server] numeric (base64), servername, descr");


	if ((conn = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		POSIXERR("Could not create socket");

	setsockopt(conn, IPPROTO_TCP, TCP_NODELAY, &flag_set, sizeof(flag_set));
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(uplink);
	server.sin_port = htons(atoi(port));       /* server port */

	if (connect(conn, (struct sockaddr *) &server, sizeof(server)) < 0)
		POSIXERR("Failed to connect to server");
	net_greet(pass, servername, descr);
}

void send_raw(char *str) {
	char buf[513];
	ssize_t tosend, sent;
	tosend = strlen(str);

	logfmt(LOG_RAW, "-> %s", str);
	while (tosend) {
		sent = send(conn, str, tosend, 0);
		if (sent < 0)
			POSIXERR("Error in sending data");
		tosend -= sent;
	}
	handle_input(strbufcpy(buf, str));
}

void send_words(const char *first, ...) {
	char buf[513];
	struct manyargs arg;
	char *word;
	va_list ap;

	va_start(ap, first);
	arg.c = 0;
	arg.v[arg.c++] = (char *)first;
	while ((word = va_arg(ap, char *)))
		arg.v[arg.c++] = word;
	va_end(ap);
	send_raw(rfc_join(buf, sizeof(buf), arg.c, arg.v));
}

void send_format(const char *fmt, ...) {
	static char buf[512];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	send_raw(buf);
}

static void net_greet(const char *pass, const char *name, const char *descr) {
	send_format("PASS :%s\r\n", pass);
	send_format("SERVER %s 1 %ld %ld J10 GE]]] +hsn :%s\r\n", name, now, now, descr);
}

void net_read(void) {
	static char buf[65536] = { 0 };
	ssize_t oldlen, r;
	char *line, *lineend;
	struct timeval tv = { 1, 0 };
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(conn, &fds);

	oldlen = strlen(buf);
	logfmt(LOG_DEBUGIO, "---- Old (%d): '%s' ----\n", oldlen, buf);
	logtxt(LOG_DEBUGIO, "Reading/Waiting...");
	r = select(conn+1, &fds, NULL, NULL, &tv);
	if (r <= 0) {
		if (r < 0)
			POSIXERR("select() error");
		return;
	}
	r = read(conn, buf+oldlen, sizeof(buf)-oldlen-1); /* account for \0 we add later */
	logfmt(LOG_DEBUGIO, "%d bytes...\n", r);
	if (r == 0) {
		error("EOF");
		return;
	}
	buf[oldlen+r] = '\0';
	logfmt(LOG_DEBUGIO, "---- New (%d) buf[%d] ----\n", r, oldlen+r);
	logfmt(LOG_DEBUGIO, "%s\n", buf+oldlen);
	logtxt(LOG_DEBUGIO, "------------------\n");
	line = buf;
	while (*line && (lineend = strpbrk(line, "\r\n"))) {
		while (*lineend == '\r' || *lineend == '\n')
			*lineend++ = '\0';
		logfmt(LOG_RAW, "<- %s\n", line);
		handle_input(line);
		logfmt(LOG_DEBUGIO, "line = buf[%td], lineend = buf[%td]\n", line-buf, lineend-buf);
		logfmt(LOG_DEBUGIO, "line = '%s', lineend = '%s'\n", line, lineend);
		line = lineend;
	}
	logfmt(LOG_DEBUGIO, "---- NewOld (%zd): '%s' '%2X %2X %2X'----\n", strlen(line), line,
		   (unsigned char)line[strlen(line)-3], (unsigned char)line[strlen(line)-2], (unsigned char)line[strlen(line)-1]);
	memmove(buf, line, strlen(line)+1);
}
