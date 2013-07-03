#include <stdlib.h>
#include <string.h>

#include "log.h"

void sfree(void *p) {
	if (!p) {
		debug(LOG_WARNING, "Attempted to free null pointer");
		return;
	}
	free(p);
}

void *smalloc(size_t s) {
	void *p = malloc(s);
	if (!p) {
		debug(LOG_WARNING, "Out of memory");
	}
	return p;
}

void *zmalloc(size_t s) {
	void *p = smalloc(s);
	if (p)
		memset(p, 0, s);
	return p;
}

char *strncpyz(char *dest, const char *src, size_t n) {
	strncpy(dest, src, n);
	if (n > 0)
		dest[n-1] = '\0';
	return dest;
}
