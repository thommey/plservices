#ifndef TCLBASE_H_
#define TCLBASE_H_

#define TCL_BASE "plservices"
#define PATHLEN 255

/* Interpreter Helpers */
static void tclpanic(const char *format, ...);
int tcl_init(void);
int tcl_load_script(char *script);
int tcl_unload_script(char *script);
Tcl_Interp *tcl_find_interp_by_script(char *script);
int tcl_valid_script(const char *script);
void tcl_onprivmsg(struct user *from, struct user *to, char *msg);
int tcl_insert_script(Tcl_Interp *interp, char *fileName);

/* Tcl Export Functions */
int tcl_local_client_create(ClientData cd, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_local_client_destroy(ClientData cd, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_local_client_join(ClientData cd, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_local_client_part(ClientData cd, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_local_client_privmsg(ClientData cd, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
int tcl_local_client_notice(ClientData cd, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]);
void tcl_init_commands(Tcl_Interp *interp);

/* TCL Invoke */
int Tcl_Invoke TCL_VARARGS_DEF(Tcl_Interp *, arg1);
#endif
