#ifndef MEM_H_
#define MEM_H_

/* memory accounting per module */
#ifndef MODNAME
#define MODNAME "core"
#endif

#undef malloc
#undef zmalloc
#undef free
#undef strdup

void *mem_malloc(const char *modname, size_t s);
void *mem_zmalloc(const char *modname, size_t s);
char *mem_strdup(const char *modname, const char *str);
void mem_free(const char *modname, void *p);

#define malloc(x) mem_malloc(MODNAME, x)
#define zmalloc(x) mem_zmalloc(MODNAME, x)
#define free(x) mem_free(MODNAME, x)
#define strdup(x) mem_strdup(MODNAME, x)

void mem_print(void);

#endif // MEM_H_
