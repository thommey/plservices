#ifndef TOKENS_H_
#define TOKENS_H_

void add_token(char *shortname, char *name, char *rulestr1, char *rulestr2, void (*handler1)(void), void (*handler2)(void));
void init_tokens(void);
void memreport_tokens(void);
void handle_input(char *str);
void set_registered(int reg);

#endif // TOKENS_H_
