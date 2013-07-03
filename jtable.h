#ifndef JTABLE_H_
#define JTABLE_H_

#include <Judy.h>

typedef Pvoid_t jtable;

/* string indexed table */
void *jtableS_insert(jtable *table, char *key, void *data);
void *jtableS_get(jtable *table, char *key);
int jtableS_remove(jtable *table, char *key);

/* pointer indexed table without data (0/1 only) */
int jtableP_set(jtable *table, void *key);
int jtableP_check(jtable *table, void *key);
int jtableP_unset(jtable *table, void *key);
int jtableP_free(jtable *table);
int jtableP_count(jtable *table);
void jtableP_iterate(jtable *table, void (*f)(void *key));

#endif // JTABLE_H_
