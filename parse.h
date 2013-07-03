#ifndef PARSE_H_
#define PARSE_H_

/* maximum number of real arguments in a command (after parsing) */
#define ARGSMAX 20

/* structures to hold an array of char* with their count in c,
 * v[i] might be misused to hold a pointer to such a struct itself
 * in case of a greedy argument */
struct manyargs {
	unsigned char c;
	void *v[256];
};

struct args {
	unsigned char c;
	void *v[ARGSMAX];
};

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
	void *(*convert)(char *);
};

/* array of individual rules (e.g. 1 o2 g3 o-1) */
struct parserule {
	unsigned char c;
	struct argrule r[ARGSMAX];
};

struct manyargs *rfc_split(char *line);
struct manyargs *commasplit(char *line);
char *rfc_join(struct manyargs *arg, int forcecolon);
struct args *arrange_args(struct manyargs *args, struct parserule *rule, int skip);
void call_handler(void * from, int argc, void **v, void (*f)(void));

#endif // PARSE_H_
