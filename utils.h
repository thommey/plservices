#ifndef UTILS_H_
#define UTILS_H_

#define strbufcpy(d, s) strncpyz((d), (s), sizeof(d))

void sfree(void *p);
void *smalloc(size_t s);
void *zmalloc(size_t s);
char *strncpyz(char *dest, const char *src, size_t n);

#endif // UTILS_H_
