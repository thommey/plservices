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

#endif
