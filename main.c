#include <stdio.h>
#include <string.h>
#include <time.h>

#include "tokens.h"
#include "net.h"

time_t now;

int main(int argc, char **argv)
{
	init_tokens();
	now = time(NULL);
	net_connect("127.0.0.1", "4400");
	now = time(NULL);
	send_raw("PASS :pass");
	send_format("SERVER lua.services.metairc.net 1 %ld %ld J10 AC]]] :Lua Services", now, now);
	now = time(NULL);
	while (1) {
		net_read();
	}
	return 0;
}
