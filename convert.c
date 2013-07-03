#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "servers.h"
#include "users.h"
#include "entities.h"
#include "parse.h"
#include "log.h"

#define CACHESIZE 32

static int intcache[CACHESIZE];
static long longcache[CACHESIZE];
static time_t timecache[CACHESIZE];

static size_t intcacheidx;
static size_t longcacheidx;
static size_t timecacheidx;

struct longcache {
	int isused;
	long data;
};

struct timecache {
	int isused;
	time_t data;
};

struct user *convert_nick(char *str) {
	return NULL;
}

struct user *convert_unum(char *str) {
	if (strlen(str) != UNUMLEN)
		return NULL;

	return get_user_by_numeric(str);
}

struct server *convert_snum(char *str) {
	if (strlen(str) != SNUMLEN)
		return NULL;

	return get_server_by_numeric(str);
}

struct entity *convert_num(char *str) {
	if (!str || !str[0])
		return NULL;

	if (strlen(str) == SNUMLEN)
		return (struct entity *)convert_snum(str);
	if (strlen(str) == UNUMLEN)
		return (struct entity *)convert_unum(str);

	return NULL;
}

struct channel *convert_chan(char *str) {
	return NULL;
}

struct ip *convert_ip(char *str) {
	return NULL;
}

int *convert_int(char *str) {
	if (intcacheidx >= CACHESIZE)
		error("intcache exhausted");

	intcache[intcacheidx] = (int)strtol(str, NULL, 10);

	return &intcache[intcacheidx++];
}

long *convert_long(char *str) {
	if (intcacheidx >= CACHESIZE)
		error("longcache exhausted");

	longcache[intcacheidx] = strtol(str, NULL, 10);

	return &longcache[longcacheidx++];
}

time_t *convert_time(char *str) {
	if (timecacheidx >= CACHESIZE)
		error("timecache exhausted");

	timecache[timecacheidx] = (time_t)strtoll(str, NULL, 10);

	return &timecache[timecacheidx++];
}

void free_conversion(void) {
	intcacheidx = 0;
	longcacheidx = 0;
	timecacheidx = 0;
}
