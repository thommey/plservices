#ifndef BASE64_H_
#define BASE64_H_

#include <stddef.h>

#define UNUMLEN 5
#define SNUMLEN 2

void init_base64(void);
long base64_decode_long(const char *str, size_t len);
char *base64_encode_padded(char *buf, size_t bufsize, unsigned long num);
int base64_incr(char *buf, size_t bufsize);
unsigned long str2unum(const char *str);
unsigned long str2snum(const char *str);
const char *unum2str(unsigned long unum);
const char *snum2str(unsigned long snum);
const char *cnum2str(unsigned long snum, unsigned long cnum);

#endif // BASE64_H_
