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
**/

#ifndef JTABLE_H_
#define JTABLE_H_

#include <Judy.h>

typedef Pvoid_t jtable;

/* string indexed table */
void *jtableS_insert(jtable *table, const char *key, void *data);
void *jtableS_get(jtable *table, const char *key);
int jtableS_remove(jtable *table, const char *key);
void jtableS_iterate0(jtable *table, void (*f)(const char *key, void *data));
void jtableS_iterate1(jtable *table, void (*f)(const char *key, void *data, void *arg), void *arg);

/* pointer indexed table without data (0/1 only) */
int jtableP_set(jtable *table, void *key);
int jtableP_check(jtable *table, void *key);
int jtableP_unset(jtable *table, void *key);
int jtableP_free(jtable *table);
int jtableP_count(jtable *table);
void jtableP_iterate1(jtable *table, void (*f)(void *arg, void *key), void *arg);
void jtableP_iterate0(jtable *table, void (*f)(void *key));

#endif // JTABLE_H_
