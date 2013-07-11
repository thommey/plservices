#ifndef BASE64_H_
#define BASE64_H_

void init_base64(void);
long base64_decode_long(char *str, size_t len);
char *base64_encode_padded(long num, char *buf, size_t bufsize);
int base64_incr(char *buf, size_t bufsize);

#endif // BASE64_H_
