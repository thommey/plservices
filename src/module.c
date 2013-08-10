/*
 * module.c - Module management.
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
 */

#include <dlfcn.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <main.h>

int module_register(void *module, const char *name, const char *description) {
	int index = 0;
        for (; modules[index].module != NULL; index++);
        if (index > MAX_MODULES) {
                logfmt(LOG_WARNING, "Cannot load module '%s' - max modules limit exceeded.", name);
		return 1;
	}
	modules[index].module = module;
	modules[index].name = name;
	modules[index].description = description;
	logfmt(LOG_DEBUG, "Module added - index: %d - name: %s - description: %s\n", index, name, description);
        return 0;
}

int module_unregister(void *module) { 
	int position = -1;
	for (int x = 0; x < MAX_MODULES; x++) { 
		if (modules[x].module == module) { 
			position = x;
			break;
		}
	}
        for (int c = position; c < MAX_MODULES - 1 ; c++) modules[c] = modules[c+1];
	return 1;
}

int module_unload(const char *name) {
        void *mod = module_find_by_name(name);
        if (mod == NULL) return 1;
        module_func unload;
        unload = dlsym(mod, "unload");
        if (dlerror()) {
                logfmt(LOG_ERROR, "Cannot call 'unload' for %s - error: %s\n", name, dlerror());
                return 1;
        }
	module_unregister(mod);
        int result = unload();
        return result;
}

int module_load(const char *path, const char *name, const char *description) {
	logfmt(LOG_DEBUG, "(%s): Loading...\n", name);
        void *module;
        module = dlopen(path, RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND);
        if (!module) {
                logfmt(LOG_ERROR, "Cannot load %s - error: %s\n", name, dlerror());
                return 1;
        }
        module_func init;
        init = dlsym(module, "load");
        if (dlerror()) {
 	       logfmt(LOG_ERROR, "Cannot call 'load' for %s - error: %s\n", path, dlerror());
               return 1;
        }
        int res = init();
	if (res > 0) {
		logfmt(LOG_ERROR, "(%s): load() was called, and exited with error.\n", name);
		return 1;
	}
	module_register(module, name, description);
        return 0;
}

void * module_find_by_name(const char *name) { 
	for (int x = 0; x < MAX_MODULES; x++) {
		if (!strncmp(modules[x].name, name, strlen(name))) { 
			return modules[x].module;
		}
	}
	return NULL;
}

const char *module_get_description(const char *name) { 
        for (int x = 0; x < MAX_MODULES; x++) {
                if (!strncmp(modules[x].name, name, strlen(name))) {
                        return modules[x].description;
                }
        }
        return NULL;
}

void module_loadAll() { 
	module_load("src/modules/dummy.so", "dummy", "PLServices Dummy Module.");
}
