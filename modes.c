#include <string.h>
#include "modes.h"

static struct modeinfo chanmodelist[128];
static struct modeinfo usermodelist[128];

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

struct modeinit {
	char *modes;
	char type;
};

static struct modeinit chanmodetable[] = {
	{ "b", MODE_BAN },
	{ "k", MODE_KEY },
	{ "l", MODE_LIMIT },
	{ "imnpstrDducCNMT", MODE_FLAG },
	{ "ov", MODE_PREFIX },
	{ NULL, 0 }
};

static struct modeinit usermodetable[] = {
	{ NULL, 0 }
};

void mode_set(uint64_t *modes, char modechar) {
	*modes |= ((uint64_t)1 << mode2offset(modechar));
}

int mode_check(uint64_t *modes, char modechar) {
	return *modes & ((uint64_t)1 << mode2offset(modechar));
}

void mode_unset(uint64_t *modes, char modechar) {
	*modes &= ~((uint64_t)1 << mode2offset(modechar));
}

void registermode(struct modeinfo *modelist, char modechar, char flags) {
	modelist[(unsigned char)modechar].flags = flags;
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
