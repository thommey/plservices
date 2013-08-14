#ifndef CONFIG_H_
#define CONFIG_H_

int load_config(const char *path);
char *config_get(const char *section, const char *key);
void config_set(const char *section, const char *key, const char *value);
void print_config(void);

#endif // CONFIG_H_
