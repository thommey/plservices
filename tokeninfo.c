#include <string.h>

#include "jtable.h"

static jtable tokeninfos = (jtable)NULL;

struct tokeninfo *get_tokeninfo(char *token) {
	return jtableS_get(&tokeninfos, token);
};

struct tokeninfo *add_tokeninfo(char *token, struct tokeninfo *info) {
	return jtableS_insert(&tokeninfos, token, info);
};
