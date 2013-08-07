/**
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

#ifndef JTABLE_H_
#define JTABLE_H_

#include <Judy.h>

typedef struct { void *t; } jtableP;
typedef struct { void *t; } jtableL;
typedef struct { void *t; } jtableS;

typedef void (*jtableS_cb)(char *key, void *data, void *param);
typedef void (*jtableL_cb)(unsigned long key, void *data, void *param);
typedef void (*jtableP_cb)(void *key, void *param);

/* string indexed table */
void *jtableS_insert(jtableS *table, const char *key, void *data);
void *jtableS_get(jtableS *table, const char *key);
int jtableS_remove(jtableS *table, const char *key);
int jtableS_free(jtableS *table);
void jtableS_iterate(jtableS *table, jtableS_cb f, void *param);

/* ulong indexed table */
void *jtableL_insert(jtableL *table, unsigned long key, void *data);
void *jtableL_get(jtableL *table, unsigned long key);
int jtableL_remove(jtableL *table, unsigned long key);
int jtableL_free(jtableL *table);
void jtableL_iterate(jtableL *table, jtableL_cb f, void *param);

/* pointer indexed table without data (0/1 only) */
int jtableP_set(jtableP *table, void *key);
int jtableP_check(jtableP *table, void *key);
int jtableP_unset(jtableP *table, void *key);
int jtableP_free(jtableP *table);
int jtableP_count(jtableP *table);
void jtableP_iterate(jtableP *table, jtableP_cb f, void *param);

#endif /* JTABLE_H_ */
