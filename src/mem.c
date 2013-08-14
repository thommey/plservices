/**
 * mem.c - Memory allocation, deallocation and accounting
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

#include "main.h"

#undef malloc
#undef free
#undef strdup

static jtableS memorydata, totalmem;

static void mem_print_mod(const char *modname, unsigned long total, void *nothing) {
	logfmt(LOG_MEMORY, "Memory allocated for %s: %lu", modname, total);
}

void mem_print(void) {
	jtableS_iterate(&totalmem, (jtableS_cb)mem_print_mod, NULL);
}

static void sub_total(const char *modname, unsigned long change) {
	unsigned long *total;
	total = jtableS_getptr(&totalmem, modname);
	assert(total);
	*total -= change;
}

static void add_total(const char *modname, long change) {
	unsigned long *total;
	total = jtableS_getptr(&totalmem, modname);
	if (!total)
		total = jtableS_insert_getptr(&totalmem, modname, (void *)0);
	*total += change;
}

static void unset_mem(const char *modname, void *p) {
	unsigned long s;
	jtableL *modmemory;

	modmemory = jtableS_getptr(&memorydata, modname);
	if (!modmemory) {
		logfmt(LOG_WARNING, "Attempted to free memory data in unknown module: %s", modname);
		return;
	}
	s = (unsigned long)jtableL_get(modmemory, (unsigned long)p);
	jtableL_remove(modmemory, (unsigned long)p);
	sub_total(modname, s);
}

static void set_mem(const char *modname, void *p, unsigned long s) {
	jtableL *modmemory;

	modmemory = jtableS_getptr(&memorydata, modname);
	if (!modmemory)
		modmemory = jtableS_insert_getptr(&memorydata, modname, NULL);
	jtableL_insert(modmemory, (unsigned long)p, (void *)s);
	add_total(modname, s);
}

void *mem_malloc(const char *modname, size_t s) {
	void *p = malloc(s);
	if (!p) {
		logfmt(LOG_FATAL, "Out of memory. Could not allocate %zd bytes.", s);
		exit(1);
	}
	set_mem(modname, p, s);
	return p;
}

void *mem_zmalloc(const char *modname, size_t s) {
	void *p = mem_malloc(modname, s);
	if (p)
		memset(p, 0, s);
	return p;
}

char *mem_strdup(const char *modname, const char *str) {
	char *new;
	size_t s = strlen(str);
	new = mem_malloc(modname, s+1);
	strcpy(new, str);
	return new;
}

void mem_free(const char *modname, void *p) {
	unset_mem(modname, p);
	free(p);
}
