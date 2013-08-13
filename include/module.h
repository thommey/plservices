#ifndef MODULE_H_
#define MODULE_H_

#define MAX_MODULES 100

/* Type definition for modul_func which we use for executing functions */
typedef int (*module_func)();

/* Module info struct */
struct moduleInfo {
        void *module;
        const char *name;
        const char *description;
};

/* Module Info Struct Array */
struct moduleInfo modules[MAX_MODULES];

/* Function declarations */

int module_add(void *module, const char *name, const char *description);
extern int module_load( const char *path, const char *name, const char *description);
extern int module_unload(const char *name);
void * module_find_by_name(const char *name);
void module_loadAll(void);
int module_unregister(void *module);

/* Helpers */
void module_join_channel(struct user *from, const char *channel, int auto_op);
void module_part_channel(struct user *from, const char *channel);
struct user *module_create_client(char *nick, const char *ident, const char *hostname, char *modes, char *account, char *opername, const char *realname);
struct user *module_create_client_on(struct server *server, char *nick, const char *ident, const char *hostname, char *modes, char *account, char *opername, const char *realname);
void module_destroy_client(struct user *from, const char *message);
void module_privmsg(struct user *from, const char *target, const char *message, ...);
void module_describe(struct user *from, const char *target, const char *message, ...);
void module_notice(struct user *from, const char *target, const char *message, ...);
#endif
