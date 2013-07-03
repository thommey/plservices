#include <stdarg.h>
#include <stdio.h>

#include "log.h"

static int debuglvl = LOG_DEBUG|LOG_ERROR|LOG_FATAL|LOG_WARNING;

void debug(int loglevel, const char *fmt, ...) {
	va_list ap;
	if (!(debuglvl & loglevel))
		return;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}
