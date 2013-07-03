#ifndef NET_H_
#define NET_H_

void net_connect(const char *servername, const char *port);
void send_format(const char *format, ...);
void send_words(int forcecolon, ...);
void send_raw(char *str);
void net_read(void);

#define send_raw(s) send_raw(s "\r\n")
#define send_format(f, ...) send_format(f "\r\n", __VA_ARGS__)
#define send_words(f, ...) send_words(f, __VA_ARGS__, "\r\n", NULL)

#endif // NET_H_
