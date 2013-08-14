/**
 * config.c - Config file handling, ini file format without quotes
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

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "main.h"

#define SECTIONLEN 63
#define LINELEN 512

static jtableS configdata;

static void skip(char **str, char *delim) {
	if (!*str)
		return;
	*str += strspn(*str, delim);
}

static int set_section(char *section, char *line) {
	skip(&line, "[ ");
	if (!strtok(line, "] ")) {
		logfmt(LOG_WARNING, "Invalid section header in configfile: %s", line);
		return -1;
	}
	strncpyz(section, line, SECTIONLEN+1);
	return 0;
}

char *config_get(const char *section, const char *key) {
	jtableS *sectiondata;
	if (!section || !key || !section[0] || !key[0]) {
		logtxt(LOG_WARNING, "Invalid config file get with empty data");
		return NULL;
	}
	sectiondata = jtableS_getptr(&configdata, section);
	if (!sectiondata)
		return NULL;
	return jtableS_get(sectiondata, key);
}

void config_set(const char *section, const char *key, const char *value) {
	jtableS *sectiondata;
	char *tmp;

	if (!section || !key || !value || !section[0] || !key[0] || !value[0]) {
		logtxt(LOG_WARNING, "Invalid config file insert with empty data");
		return;
	}
	sectiondata = jtableS_getptr(&configdata, section);
	if (!sectiondata) {
		jtableS_insert(&configdata, section, NULL);
		sectiondata = jtableS_getptr(&configdata, section);
	}
	tmp = jtableS_get(sectiondata, key);
	/* if overwriting old value, free old string first */
	free(tmp);
	jtableS_insert(sectiondata, key, strdup(value));
}

static int parse_configline(char *section, char *line) {
	char *value;
	/* skip leading spaces */
	skip(&line, " ");
	/* ignore empty lines and comments */
	if (!*line || *line == '#')
		return 0;
	/* a section header */
	if (*line == '[') {
		set_section(section, line);
		return 0;
	}
	/* first ' ' or '=' terminates key */
	value = strpbrk(line, "= ");
	*value++ = '\0';
	skip(&value, " =");
	if (!value) {
		logfmt(LOG_WARNING, "Config file parser found malformed line: %s", line);
		return -1;
	}
	/* cut off at line end */
	strtok(value, "\r\n");
	config_set(section, line, value);
	return 0;
}

int load_config(const char *path) {
	char line[LINELEN+1], section[SECTIONLEN+1];
	FILE *f;
	if (!(f = fopen(path, "r"))) {
		POSIXERR("opening config file");
		error("Error opening config file");
	}
	strbufcpy(section, "core");
	while (fgets(line, sizeof(line), f))
		parse_configline(section, line);
	return 0;
}

static void print_key_value(char *key, char *value, char *section) {
	logfmt(LOG_DEBUG, "%s = %s", key, value);
}

static void print_section(char *section, jtableS sectiondata) {
	logfmt(LOG_DEBUG, "[%s]", section);
	jtableS_iterate(&sectiondata, (jtableS_cb)print_key_value, section);
}

void print_config(void) {
	jtableS_iterate(&configdata, (jtableS_cb)print_section, NULL);
}
