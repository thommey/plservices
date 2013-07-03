#ifndef MODES_H_
#define MODES_H_

typedef uint64_t chanmode;
typedef uint64_t usermode;

typedef int (*modehook)(struct entity *from, struct entity *target, int pls, char modechange, char *param);

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

#endif // MODES_H_
