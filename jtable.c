#include <Judy.h>

#include "jtable.h"

void *jtableS_insert(jtable *table, char *key, void *data) {
	PWord_t PValue;

	JSLI(PValue, *table, (uint8_t*)key);
	*PValue = (Word_t)data;
	return data;
}

void *jtableS_get(jtable *table, char *key) {
	PWord_t PValue;
	JSLG(PValue, *table, (uint8_t*)key);

	return PValue ? (void *)*PValue : NULL;
}

int jtableS_remove(jtable *table, char *key) {
	int ret;

	JSLD(ret, *table, (uint8_t*)key);
	return ret;
}

int jtableP_set(jtable *table, void *key) {
	int ret;

	J1S(ret, *table, (Word_t)key);
	return ret;
}

int jtableP_check(jtable *table, void *key) {
	int ret;

	J1T(ret, *table, (Word_t)key);
	return ret;
}

int jtableP_unset(jtable *table, void *key) {
	int ret;

	J1U(ret, *table, (Word_t)key);
	return ret;
}

int jtableP_free(jtable *table) {
	int ret;

	J1FA(ret, *table);
	return ret;
}

int jtableP_count(jtable *table) {
	int ret;

	J1C(ret, *table, 0, -1);
	return ret;
}

void jtableP_iterate(jtable *table, void (*f)(void *key)) {
	int ret;
	Word_t key;

	J1F(ret, *table, key);
	while (ret) {
		f((void *)key);
		J1N(ret, *table, key);
	}
}
