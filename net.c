#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"
#include "parse.h"
#include "tokens.h"

#define POSIXERR(str) do { debug(LOG_FATAL, "POSIX error: %s", strerror(errno)); error(str); } while(0)

static int conn;

extern int now;

void net_connect(const char *servername, const char *port) {
	struct sockaddr_in server;

	if ((conn = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		POSIXERR("Could not create socket");

    memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(servername);
	server.sin_port = htons(atoi(port));       /* server port */

	if (connect(conn, (struct sockaddr *) &server, sizeof(server)) < 0)
		POSIXERR("Failed to connect to server");
}

void send_raw(char *str) {
	ssize_t tosend, sent;
	tosend = strlen(str);

	debug(LOG_DEBUG, "-> %s", str);
	while (tosend) {
		sent = send(conn, str, tosend, 0);
		if (sent < 0)
			POSIXERR("Error in sending data");
		tosend -= sent;
	}
}

void send_words(int forcecolon, ...) {
	struct manyargs arg;
	char *word;
	va_list ap;

	va_start(ap, forcecolon);
	arg.c = 0;
	while ((word = va_arg(ap, char *)))
		arg.v[arg.c++] = word;
	va_end(ap);
	send_raw(rfc_join(&arg, forcecolon));
}

void send_format(const char *fmt, ...) {
	static char buf[512];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	send_raw(buf);
}

void net_read(void) {
	static char buf[513] = { 0 };
	ssize_t oldlen, r;
	char *line, *lineend;
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(conn, &fds);

	oldlen = strlen(buf);
	debug(LOG_DEBUGIO, "---- Old (%d): '%s' ----\n", oldlen, buf);
	debug(LOG_DEBUGIO, "Reading/Waiting...");
	r = select(conn+1, &fds, NULL, NULL, NULL);
	now = time(NULL);
	if (r <= 0) {
		POSIXERR("select() error");
		return;
	}
	r = read(conn, buf+oldlen, sizeof(buf)-oldlen-1); /* account for \0 we add later */
	debug(LOG_DEBUGIO, "%d bytes...\n", r);
	if (r == 0) {
		error("EOF");
		return;
	}
	buf[oldlen+r] = '\0';
	debug(LOG_DEBUGIO, "---- New (%d) buf[%d] ----\n", r, oldlen+r);
	debug(LOG_DEBUGIO, "%s\n", buf+oldlen);
	debug(LOG_DEBUGIO, "------------------\n");
	line = buf;
	while (*line && (lineend = strpbrk(line, "\r\n"))) {
		while (*lineend == '\r' || *lineend == '\n')
			*lineend++ = '\0';
		debug(LOG_DEBUG, "<- %s\n", line);
		handle_input(line);
		debug(LOG_DEBUGIO, "line = buf[%td], lineend = buf[%td]\n", line-buf, lineend-buf);
		debug(LOG_DEBUGIO, "line = '%s', lineend = '%s'\n", line, lineend);
		line = lineend;
	}
	debug(LOG_DEBUGIO, "---- NewOld (%zd): '%s' '%2X %2X %2X'----\n", strlen(line), line,
			(unsigned char)line[strlen(line)-3], (unsigned char)line[strlen(line)-2], (unsigned char)line[strlen(line)-1]);
	memmove(buf, line, strlen(line)+1);
}
