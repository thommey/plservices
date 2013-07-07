/**
 * tokens.c - P10 token parsing and conversion rules
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

#include <string.h>

#include "main.h"
#include "convert.h"
#include "handle.h"

static int registered;

struct tokeninfo {
	struct parserule rules[2];
	void (*handlers[2])(void);
};

/* single rule, if there's no short token version */
#define MKSRULE(long, rule) { NULL, #long, h ## long, NULL, rule, "" }
/* normal rule, short and long version of the token */
#define MKLRULE(short, long, rule) { #short, #long, (void (*)(void))h ## long, NULL, rule, "" }
/* complicated rule with 2 handle functions and rulesets (NICK, ..) */
#define MK2RULE(short, long, rule1, rule2) { #short, #long, (void (*)(void))h ## long ## 1, ((void (*)(void))h ## long ## 2), rule1, rule2 }

/* rule words: [flag]<integer>[type]
 * <integer> index to take the argument from, counting starts at 1,
 *           negative is from the end (1 = first, -1 = last)
 * [type] optional type used to convert, see types list below.
 * types: unum = user numeric, snum = server numeric, uint = unsigned integer,
 *        long = long, time = timestamp, chan = channel
 * [flag] flag for the parsing rule, see flags list below.
 * flags: o = optional, g = greedy.
 */
static struct {
	char *shortname;
	char *name;
	void (*handler1)(void);
	void (*handler2)(void);
	char rule1[ARGSMAX*4];
	char rule2[ARGSMAX*4];
} rawrules[] = {
	MKLRULE(AC,	ACCOUNT,	"1unum 2"),
	MKLRULE(AD,	ADMIN,		"1snum"),
	MKLRULE(LL,	ASLL,		"1 2snum"),
	MKLRULE(A,	AWAY,		"o-1"),
	MKLRULE(B,	BURST,		"1 2time g3"),
	MKLRULE(CM,	CLEARMODE,	"1chan 2"),
/*	MKSRULE(	CLOSE,		""), */
/* 	MKLRULE(CN,	CNOTICE,	""), */
 	MKLRULE(CO,	CONNECT,	"1 2uint 3snum"),
/*	MKLRULE(CP,	CPRIVMSG,	""), */
	MKLRULE(C,	CREATE,		"1 2time"),
	MKLRULE(DE,	DESTRUCT,	"1cnum 2time"),
	MKLRULE(DS,	DESYNCH,	"-1"),
/*	MKSRULE(	DIE,		""), */
/*	MKSRULE(	DNS,		""), */
	MKLRULE(EB,	END_OF_BURST,""),
	MKLRULE(EA,	EOB_ACK,	""),
	MKLRULE(Y,	ERROR,		"o-1"),
/*	MKSRULE(	GET,		""), */
	MK2RULE(GL,	GLINE,		"1snum 2 3time -1", "1snum 2 3time 4time -1"),
/*	MKSRULE(	HASH,		""), */
/*	MKSRULE(	HELP,		""), */
	MKLRULE(F,	INFO,		"1snum"),
	MKLRULE(I,	INVITE,		"1 2chan"),
/*	MKSRULE(	ISON,		""), */
	MKLRULE(J,	JOIN, 		"1chan o2time"),
	MKLRULE(JU,	JUPE, 		"1snum 2 3time 4time -1"),
	MKLRULE(K,	KICK,		"1chan 2unum -1"),
	MKLRULE(D,	KILL,		"1unum -1"),
	MKLRULE(LI,	LINKS, 		"1snum 2"),
/*	MKSRULE(	LIST, 		""), */
	MKLRULE(LU,	LUSERS, 	"1 2snum"),
/*	MKSRULE(	MAP, 		""), */
	MK2RULE(M,	MODE,		"1nick g2", "1chan g2 o-1time"),
	MKLRULE(MO,	MOTD,		"1snum"),
	MKLRULE(E,	NAMES, 		"1chan 2snum"),
	MK2RULE(N,	NICK,		"1 2time", "1 2uint 3time 4 5 g6 -3 -2 -1"),
	MKLRULE(O,	NOTICE, 	"1 -1"),
/*	MKSRULE(	OPER,		""), */
	MKLRULE(OM,	OPMODE, 	"1chan g2 o-1time"),
	MKLRULE(L,	PART,		"1 o-1"),
	MKLRULE(PA,	PASS,		"-1"),
	MKLRULE(G,	PING,		"g1"),
	MKLRULE(Z,	PONG,		"1snum g2"),
/*	MKSRULE(	POST,		""), */
	MKLRULE(P,	PRIVMSG,	"1 -1"),
/*	MKSRULE(	PRIVS,		""), */
/*	MKSRULE(	PROTO,		""), */
	MKLRULE(Q,	QUIT,		"-1"),
/*	MKSRULE(	REHASH,		""), */
/*	MKSRULE(	RESET,		""), */
/*	MKSRULE(	RESTART,	""), */
	MK2RULE(RI,	RPING,		"1 2snum 3", "1snum 2unum 3time 4time o5"),
	MK2RULE(RO,	RPONG,		"1 2unum 3time 4time o5", "1unum 2 3long o4"),
	MKLRULE(S,	SERVER,		"1 2uint 3time 4time 5 6 7 -1"),
/*	MKSRULE(	SET,		""), */
	MKLRULE(SH,	SETHOST,	"1unum 2 3"),
	MKLRULE(SE,	SETTIME,	"1time o2snum"),
	MKLRULE(U,	SILENCE,	"1 2"),
	MKLRULE(SQ,	SQUIT,		"1 2time o-1"),
	MKLRULE(R,	STATS,		"1 2snum o3"),
/*	MKLRULE(TI,	TIME,		""), */
	MKLRULE(T,	TOPIC,		"1chan o-3time o-2time -1"),
	MKLRULE(TR,	TRACE,		"1 2snum"),
	MKLRULE(UP,	UPING,		"1 2uint 3snum 4uint"),
/*	MKSRULE(	USER,		""), */
/*	MKSRULE(	USERHOST,	""), */
/*	MKSRULE(	USERIP,		""), */
	MKLRULE(V,	VERSION,	"1snum"),
	MKLRULE(WC,	WALLCHOPS,	"1chan -1"),
	MKLRULE(WA,	WALLOPS,	"-1"),
	MKLRULE(WU,	WALLUSERS,	"-1"),
	MKLRULE(WV,	WALLVOICES,	"1chan -1"),
/*	MKLRULE(H,	WHO,		""), */
	MKLRULE(W,	WHOIS,		"1snum 2"),
/*	MKLRULE(X,	WHOWAS,		""), */
	{NULL, NULL, NULL, NULL, "", ""}
};

/* parse a single word of an arrangement rule (e.g. "o7unum") */
static void parse_argrule(struct argrule *rule, char *rulestr) {
	char *type;

	if (rulestr[0] == 'o') {
		rule->flags = RULE_OPTIONAL;
	} else if (rulestr[0] == 'g') {
		rule->flags = RULE_GREEDY;
	} else
		rule->flags = RULE_NORMAL;

	if (rule->flags != RULE_NORMAL)
		rulestr++;

	rule->offset = (int)strtol(rulestr, &type, 10);

	if (!type[0])
		return;

#define NEWTYPE(name) do { if (!strcmp(type, #name)) { rule->convert = (void *(*)(char *))convert_ ## name; return; } } while (0)
	NEWTYPE(unum);
	NEWTYPE(snum);
	NEWTYPE(uint);
	NEWTYPE(long);
	NEWTYPE(nick);
	NEWTYPE(time);
	NEWTYPE(chan);
#undef NEWTYPE
	rule->convert = NULL;
}

void add_token_rule(char *rulestr, struct parserule *r) {
	struct manyargs arg;
	int i;

	rfc_split(&arg, rulestr);
	r->c = arg.c;

	for (i = 0; i < arg.c; i++)
		parse_argrule(r->r + i, arg.v[i]);
}

/* Add a token with a parsing rule. Individual words with numbers and
 * an optional prefix (o = optional, g = greedy), and type name after it
 * (e.g. "1 o2unum g3time o-1") */
void add_token(char *shortname, char *name, char *rulestr1, char *rulestr2, void (*handler1)(void), void (*handler2)(void)) {
	struct tokeninfo *t;

	t = zmalloc(sizeof(*t));

	add_token_rule(rulestr1, &t->rules[0]);
	t->handlers[0] = handler1;

	if (handler2) {
		add_token_rule(rulestr2, &t->rules[1]);
		t->handlers[1] = handler2;
	}

	if (shortname)
		add_tokeninfo(shortname, t);
	add_tokeninfo(name, t);
}

void init_tokens(void) {
	int i = -1;

	while (rawrules[++i].name)
		add_token(rawrules[i].shortname, rawrules[i].name, rawrules[i].rule1, rawrules[i].rule2, rawrules[i].handler1, rawrules[i].handler2);
}

void set_registered(int reg) {
	registered = reg;
}

void handle_input(char *str) {
	struct manyargs raw;
	struct args *arg;
	struct tokeninfo *info;
	int skipargs = 1 + registered;

	rfc_split(&raw, str);

	/* ignore empty lines */
	if (!raw.c)
		return;

	if (registered && raw.c < 2) {
		logfmt(LOG_WARNING, "Input message '%s' has no token", str);
		return;
	}

	/* fetch token information (rule to arrange/convert arguments + handling function) */
	info = get_tokeninfo(registered ? raw.v[1] : raw.v[0]);
	free_conversion();

	/* arrange+convert arguments */
	arg = arrange_args(raw.c - skipargs, raw.v + skipargs, &info->rules[0]);

	if (!arg) {
		/* second variant */
		if (info->handlers[1]) {
			arg = arrange_args(raw.c - skipargs, raw.v + skipargs, &info->rules[1]);
			free_conversion();

			if (!arg) {
				logtxt(LOG_WARNING, "Failed to deal last input (2 rules)");
				return;
			}
			arg->v[0] = registered ? convert_num(raw.v[0]) : NULL;
			call_varargs(info->handlers[1], arg->c, arg->v);
			return;
		}
		logtxt(LOG_WARNING, "Failed to deal with last input (1 rule)");
		return;
	}
	arg->v[0] = registered ? convert_num(raw.v[0]) : NULL;
	call_varargs(info->handlers[0], arg->c, arg->v);
}
