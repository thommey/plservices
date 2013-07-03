#include <string.h>
#include <assert.h>

#include "main.h"

static char chanmodelist[128];
static char usermodelist[128];

#define mode2offset(m) (mode2offset_table[(unsigned char)(m)])

static int8_t mode2offset_table[128] = {
	/*  0 - 31 */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/* 32 - 63 */
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	/* 64 - 95 */
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, /* A - O */
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, /* P - Z */
	/* 96 - 128 */
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* a - o */
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, /* p - z */
};

static struct {
	char *modes;
	char type;
} chanmodetable[] = {
	{ "b", MODE_BAN },
	{ "k", MODE_KEY },
	{ "l", MODE_LIMIT },
	{ "imnpstrDducCNMT", MODE_FLAG },
	{ "ov", MODE_PREFIX },
	{ NULL, 0 }
}, usermodetable[] = {
	{ "ohr", MODE_LIMIT },
	{ "wOidkXnIgxRP", MODE_FLAG },
	{ NULL, 0 }
};

static char getmodeflags(char *modelist, char modechar) {
	return modelist[(unsigned char)modechar];
}

static void registermode(char *modelist, char modechar, char flags) {
	modelist[(unsigned char)modechar] = flags;
}

void init_modes() {
	int i, j;

	for (i = 0; chanmodetable[i].modes; i++)
		for (j = 0; chanmodetable[i].modes[j]; j++)
			registermode(chanmodelist, chanmodetable[i].modes[j], chanmodetable[i].type);

	for (i = 0; usermodetable[i].modes; i++)
		for (j = 0; usermodetable[i].modes[j]; j++)
			registermode(usermodelist, usermodetable[i].modes[j], usermodetable[i].type);

}

void mode_set1(uint64_t *modes, char modechar) {
	*modes |= ((uint64_t)1 << mode2offset(modechar));
}

int mode_check1(uint64_t *modes, char modechar) {
	return *modes & ((uint64_t)1 << mode2offset(modechar));
}

void mode_unset1(uint64_t *modes, char modechar) {
	*modes &= ~((uint64_t)1 << mode2offset(modechar));
}

void mode_apply(struct entity *from, struct entity *target, uint64_t *modes, char *modechanges, struct manyargs *arg, int skip, modehook func) {
	char *modelist = verify_user(target) ? usermodelist : chanmodelist;
	char *mc = modechanges;
	char *param;
	int ret, pls = 1;

	assert(verify_user(target) || verify_channel(target));

	while (*mc) {
		param = NULL;
		switch (*mc) {
		case '+':
			pls = 1;
			break;
		case '-':
			pls = 0;
			break;
		default:
			if (!hasmode_valid(modelist, *mc)) {
				debug(LOG_WARNING, "Invalid mode char: %c in sequence %s!", *mc, modechanges);
				break;
			}
			if (pls) {
				mode_set1(modes, *mc);
				if (hasmode_setparam(modelist, *mc))
					param = skip < arg->c ? arg->v[skip++] : NULL;
			} else {
				mode_unset1(modes, *mc);
				if (hasmode_unsetparam(modelist, *mc))
					param = skip < arg->c ? arg->v[skip++] : NULL;
			}
			ret = func(from, target, pls, *mc, param);
			if (ret == MODEHOOK_OK)
				return;
			if (param && (ret == MODEHOOK_NOPARAM || ret == MODEHOOK_IGNORE))
				skip--;
			if (ret == MODEHOOK_IGNORE)
				pls ? mode_unset1(modes, *mc) : mode_set1(modes, *mc);
			break;
		}
		mc++;
	}

}
