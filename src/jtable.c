/**
 * jtable.c - Wrappers around judy tables:
 *            jtableS_* for string keyed
 *            jtableP_* for word/ptr keyed booleans
 *
 * Copyright (c) 2013 Thomas Sader (thommey)
 *
 *  This file is part of PLservices.
 *
 *  PLservices is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PLservices is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PLservices.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
**/

#include <Judy.h>

#include "log.h"
#include "jtable.h"

/* Convenience functions for judy tables. jtableS = string keyed, jtableP = word keyed 0/1 bits */

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

void jtableS_iterate0(jtable *table, void (*f)(char *key, void *data)) {
	PWord_t PValue;
	uint8_t key[512];

	key[0] = '\0';
	JSLF(PValue, *table, key);
	while (PValue) {
		f((char *)key, (void *)* PValue);
		JSLN(PValue, *table, key);
	}
}

void jtableS_iterate1(jtable *table, void (*f)(char *key, void *data, void *arg), void *arg) {
	Word_t *PValue;
	uint8_t key[512];

	key[0] = '\0';
	JSLF(PValue, *table, key);
	while (PValue) {
		f((char *)key, (void *)*PValue, arg);
		JSLN(PValue, *table, key);
	}
}

int jtableP_set(jtable *table, void *key) {
	int ret;

	J1S(ret, *table, (Word_t)key);
	if (!ret)
		logtxt(LOG_DEBUG, "Duplicate insert into jtableP");
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
	if (!ret)
		logtxt(LOG_DEBUG, "Deletion of non-existant element in jtableP");
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

void jtableP_iterate0(jtable *table, void (*f)(void *key)) {
	int ret;
	Word_t key = 0;

	J1F(ret, *table, key);
	while (ret) {
		f((void *)key);
		J1N(ret, *table, key);
	}
}

void jtableP_iterate1(jtable *table, void (*f)(void *key, void *arg), void *arg) {
	int ret;
	Word_t key = 0;

	J1F(ret, *table, key);
	while (ret) {
		f((void *)key, arg);
		J1N(ret, *table, key);
	}
}
