#ifndef TOKENINFO_H_
#define TOKENINFO_H_

struct tokeninfo *get_tokeninfo(char *token);
struct tokeninfo *add_tokeninfo(char *token, struct tokeninfo *info);

#endif // TOKENINFO_H_
