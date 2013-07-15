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

#define POSIXERR(str) do { logfmt(LOG_FATAL, "POSIX error: %s", strerror(errno)); error(str); } while(0)

static int conn;

extern int now;

extern struct server *me;

static void net_connected(const char*, const char*, const char*);

void net_connect(const char *servername, const char *port, const char *pass, const char *sname, const char *sdescr) {
	int flag = 1;
	struct sockaddr_in server;

	if ((conn = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		POSIXERR("Could not create socket");

	setsockopt(conn, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(servername);
	server.sin_port = htons(atoi(port));       /* server port */

	if (connect(conn, (struct sockaddr *) &server, sizeof(server)) < 0)
		POSIXERR("Failed to connect to server");
	net_connected(pass, sname, sdescr);
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

void send_words(int forcecolon, ...) {
	char buf[513];
	struct manyargs arg;
	char *word;
	va_list ap;

	va_start(ap, forcecolon);
	arg.c = 0;
	while ((word = va_arg(ap, char *)))
		arg.v[arg.c++] = word;
	va_end(ap);
	send_raw(rfc_join(buf, sizeof(buf), arg.c, arg.v, forcecolon));
}

void send_format(const char *fmt, ...) {
	static char buf[512];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	send_raw(buf);
}

static void net_connected(const char *pass, const char *name, const char *descr) {
	send_format("PASS :%s\r\n", pass);
	send_format("SERVER %s 1 %ld %ld J10 AC]]] +hsn :%s\r\n", name, now, now, descr);
}

void net_read(void) {
	static char buf[513] = { 0 };
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
