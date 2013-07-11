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

#ifndef PARSE_H_
#define PARSE_H_

/* rule to parse an individual argument
 * 4 -> pop argument 4 to here
 * -1 -> pop last argument to here
 * type can be RULE_OPTIONAL or RULE_GREEDY
 * OPTIONAL is self-explanatory
 * GREEDY is like the 7+ in the NICK token that eats all mode parameters
 */

#define RULE_NORMAL   0x0
#define RULE_OPTIONAL 0x1
#define RULE_GREEDY   0x2

#define rule_optional(r) (r.flags & RULE_OPTIONAL)
#define rule_greedy(r) (r.flags & RULE_GREEDY)

struct argrule {
	char offset;
	unsigned char flags;
	void (*convert)(struct funcarg *dest, char *str);
};

/* array of individual rules (e.g. 1 o2 g3 o-1) */
struct parserule {
	unsigned char c;
	struct argrule r[MAXARGS];
};

void init_parse(void);
char *rfc_tolower(char *buf, size_t bufsize, const char *str);
struct manyargs *rfc_split(struct manyargs *arg, char *line);
struct manyargs *split(struct manyargs *arg, char *line, char delim);
char *rfc_join(char *buf, size_t bufsize, int argc, char **argv, int forcecolon);
struct args *arrange_args(int argc, char **argv, struct parserule *rule);
void call_varargs(void (*f)(), struct args *arg);

#endif /* PARSE_H_ */
