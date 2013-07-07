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
**/

#ifndef MODES_H_
#define MODES_H_

struct modebuf {
	struct user *from;
	struct channel *chan;
	int modecount, lastplsmns;
	char *modestrpos, modestr[64];
	char *targetstrpos, targetstr[256];
};

typedef uint64_t chanmode;
typedef uint64_t usermode;
typedef uint64_t servermode;

typedef int (*modehook)(struct entity *from, struct entity *target, int pls, char modechange, char *param);

int mode_apply(struct entity *from, struct entity *target, uint64_t *modes, char *modechanges, struct manyargs *arg, int skip, modehook func);
void init_modes();
int mode_check1(uint64_t *modes, char modechar);
void mode_set1(uint64_t *modes, char modechar);
void mode_unset1(uint64_t *modes, char modechar);
struct modebuf *mode_pushmode(struct user *from, struct channel *c, int plsmns, char mode, const char *target, size_t targetlen);
void mode_flushmode(struct modebuf *m);

void uplink_with_opername(void);
void uplink_without_opername(void);

#define MODEFLAG_VALID      0x80
#define MODEFLAG_SETPARAM   0x01
#define MODEFLAG_UNSETPARAM 0x02
#define MODEFLAG_LIST       (0x04|MODEFLAG_SETPARAM)
#define MODEFLAG_USERMODE   (0x08|MODEFLAG_SETPARAM|MODEFLAG_UNSETPARAM)
#define MODEFLAG_NOUNSET    0x10

#define MODE_FLAG       (MODEFLAG_VALID)
#define MODE_LIMIT      (MODEFLAG_VALID|MODEFLAG_SETPARAM)
#define MODE_KEY        (MODEFLAG_VALID|MODEFLAG_SETPARAM|MODEFLAG_UNSETPARAM)
#define MODE_BAN        (MODEFLAG_VALID|MODEFLAG_LIST)
#define MODE_PREFIX     (MODEFLAG_VALID|MODEFLAG_USERMODE)
#define MODE_REGISTERED (MODEFLAG_VALID|MODEFLAG_LIST|MODEFLAG_NOUNSET)

#define hasmode_valid(ml, c) (getmodeflags((ml), (c)) & MODEFLAG_VALID)
#define hasmode_setparam(ml, c) (getmodeflags((ml), (c)) & MODEFLAG_SETPARAM)
#define hasmode_unsetparam(ml, c) (getmodeflags((ml), (c)) & MODEFLAG_UNSETPARAM)
#define hasmode_list(ml, c) (getmodeflags((ml), (c)) & MODEFLAG_LIST)
#define hasmode_usermode(ml, c) (getmodeflags((ml), (c)) & MODEFLAG_USERMODE)
#define hasmode_nounset(ml, c) (getmodeflags((ml), (c)) & MODEFLAG_NOUNSET)

#define ismode_flag(ml, c) (getmodeflags((ml), (c)) == MODE_FLAG)
#define ismode_limit(ml, c) (getmodeflags((ml), (c)) == MODE_LIMIT)
#define ismode_key(ml, c) (getmodeflags((ml), (c)) == MODE_KEY)
#define ismode_ban(ml, c) (getmodeflags((ml), (c)) == MODE_BAN)
#define ismode_prefix(ml, c) (getmodeflags((ml), (c)) == MODE_PREFIX)
#define ismode_registered(ml, c) (getmodeflags((ml), (c)) == MODE_REGISTERED)

#define MODEHOOK_OK        0
#define MODEHOOK_NOPARAM   -1
#define MODEHOOK_IGNORE    -2
#define MODEHOOK_ERROR     -3

#endif /* MODES_H_ */
