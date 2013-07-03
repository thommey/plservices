#ifndef TOKENS_H_
#define TOKENS_H_

#include "parse.h"

void add_token(const char *shortname, const char *name, char *rulestr);
void init_tokens(void);
void memreport_tokens(void);
void handle_input(char *str);
void set_registered(int reg);

#endif // TOKENS_H_
