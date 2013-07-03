#ifndef LOG_H_
#define LOG_H_

#include <stdlib.h>

#define LOG_FATAL      0x0001
#define LOG_ERROR      0x0010
#define LOG_WARNING    0x0100
#define LOG_DEBUGIO    0x2000
#define LOG_DEBUG      0x4000
#define LOG_PARSEDEBUG 0x8000

#define error(msg) do { debug(LOG_FATAL, "Fatal error at %s (line %d): %s", __FILE__, __LINE__, msg); exit(1); } while (0)
void debug(int loglevel, const char *fmt, ...);

#endif // LOG_H_
