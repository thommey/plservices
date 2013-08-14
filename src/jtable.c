/**
 * jtable.c - Wrappers around judy tables:
 *            jtableS_* for string keyed
 *            jtableL_* for word/ptr keyed
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
 */

#include <Judy.h>

#include "log.h"
#include "utils.h"
#include "args.h"
#include "parse.h"
#include "tokens.h"
#include "jtable.h"

/* Convenience functions for judy tables.
 * jtableS = string keyed values,
 * jtableL = ulong keyed values,
 * jtableP = word keyed 0/1 bits
 */

void *jtableS_insert(jtableS *table, const char *key, void *data) {
	PWord_t PValue;

	JSLI(PValue, table->t, (const uint8_t*)key);
	*PValue = (Word_t)data;
	return data;
}

void *jtableS_getptr(jtableS *table, const char *key) {
	PWord_t PValue;
	JSLG(PValue, table->t, (const uint8_t*)key);

	return PValue ? (void *)PValue : NULL;
}

void *jtableS_get(jtableS *table, const char *key) {
	PWord_t PValue;
	JSLG(PValue, table->t, (const uint8_t*)key);

	return PValue ? (void *)*PValue : NULL;
}

int jtableS_remove(jtableS *table, const char *key) {
	int ret;

	JSLD(ret, table->t, (const uint8_t*)key);
	return ret;
}

int jtableS_free(jtableS *table) {
	int ret;

	JSLFA(ret, table->t);
	return ret;
}

void jtableS_iterate(jtableS *table, jtableS_cb f, void *param) {
	PWord_t PValue;
	uint8_t key[512];

	key[0] = '\0';
	JSLF(PValue, table->t, key);
	while (PValue) {
		f((char *)key, (void *)*PValue, param);
		JSLN(PValue, table->t, key);
	}
}

void *jtableL_insert(jtableL *table, unsigned long key, void *data) {
	PWord_t PValue;

	JLI(PValue, table->t, key);
	*PValue = (Word_t)data;
	return data;
}

void *jtableL_get(jtableL *table, unsigned long key) {
	PWord_t PValue;
	JLG(PValue, table->t, key);

	return PValue ? (void *)*PValue : 0;
}

int jtableL_remove(jtableL *table, unsigned long key) {
	int ret;

	JLD(ret, table->t, key);
	return ret;
}

int jtableL_free(jtableL *table) {
	int ret;

	JLFA(ret, table->t);
	return ret;
}

void jtableL_iterate(jtableL *table, jtableL_cb f, void *param) {
	PWord_t PValue;
	Word_t key = 0UL;

	JLF(PValue, table->t, key);
	while (PValue) {
		f(key, (void *)*PValue, param);
		JLN(PValue, table->t, key);
	}
}


int jtableP_set(jtableP *table, void *key) {
	int ret;

	J1S(ret, table->t, (Word_t)key);
	if (!ret)
		logtxt(LOG_DEBUG, "Duplicate insert into jtableP");
	return ret;
}

int jtableP_check(jtableP *table, void *key) {
	int ret;

	J1T(ret, table->t, (Word_t)key);
	return ret;
}

int jtableP_unset(jtableP *table, void *key) {
	int ret;

	J1U(ret, table->t, (Word_t)key);
	if (!ret)
		logtxt(LOG_DEBUG, "Deletion of non-existant element in jtableP");
	return ret;
}

int jtableP_free(jtableP *table) {
	int ret;

	J1FA(ret, table->t);
	return ret;
}

int jtableP_count(jtableP *table) {
	int ret;

	J1C(ret, table->t, 0, -1);
	return ret;
}

void jtableP_iterate(jtableP *table, jtableP_cb f, void *param) {
	int ret;
	Word_t key = 0;

	J1F(ret, table->t, key);
	while (ret) {
		f((void *)key, param);
		J1N(ret, table->t, key);
	}
}
