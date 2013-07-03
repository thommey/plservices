#ifndef CONVERT_H_
#define CONVERT_H_

struct user *convert_nick(char *str);
struct user *convert_unum(char *str);
struct server *convert_snum(char *str);
struct entity *convert_num(char *str);
struct channel *convert_chan(char *str);
struct ip *convert_ip(char *str);
int *convert_int(char *str);
long *convert_long(char *str);
time_t *convert_time(char *str);
void free_conversion(void);

#endif // CONVERT_H_
