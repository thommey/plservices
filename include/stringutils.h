#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

#include <ctype.h> /* toupper, tolower */
int rfc_tolower(int in);

void init_stringutils(void);
char *strconv(int (*convert)(int), char *buf, size_t buflen, const char *str);

#endif // STRINGUTILS_H_
