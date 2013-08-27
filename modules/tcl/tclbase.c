#define PANICPREFIX "Tcl Panic: "

#include <tcl.h>
#include "main.h"
#include "tclbase.h"

static jtableP tcl_interps;
static jtableS tcl_interps_by_script;

/* Handler for any TCL panics that are raised. */
static void tclpanic(const char *format, ...) {
	char *newfmt;
	va_list ap;
	va_start(ap, format);
	newfmt = smalloc(strlen(format)+strlen(PANICPREFIX)+1);
	strcpy(newfmt, PANICPREFIX);
	strcat(newfmt, format);
	logfmtva(LOG_ERROR, newfmt, ap);
	va_end(ap);
}

/* Create a new instance. */
int tcl_init () {
	Tcl_FindExecutable(TCL_BASE);
	Tcl_SetPanicProc(tclpanic);
	hook_hook("onprivmsg", tcl_onprivmsg);
	return TCL_OK;
}

/* Create a new TCL interpreter and load a script. */
int tcl_load_script(char *script) {
	Tcl_Interp *interp;
	interp = Tcl_CreateInterp();
	// Export functions
	tcl_init_commands(interp);
	// Load the default script.
	char base[PATHLEN+1];
	snprintf(base, sizeof(base), "scripts/tcl/%s.tcl", config_get("mod.tcl", "base_script"));
	int baseresult = tcl_insert_script(interp, base);
	char *result = Tcl_GetString(Tcl_GetObjResult(interp));
	if (baseresult == TCL_ERROR) { logfmt(LOG_ERROR, "Couldn't load '%s' - error: %s", base, result); }
	// Try and resolve the path here.
	char path[PATHLEN+1];
	if (strpbrk(script, "./")) return TCL_ERROR;
	snprintf(path, sizeof(path), "scripts/tcl/%s.tcl", script);
	// Export some commands to the TCL commands.
	if (tcl_insert_script(interp, path)) {
		jtableP_set(&tcl_interps, interp);
		jtableS_insert(&tcl_interps_by_script, script, interp);
		if (Tcl_Init(interp) != TCL_OK) return TCL_ERROR;
		int code = Tcl_Eval(interp, "load");
		char *result = Tcl_GetString(Tcl_GetObjResult(interp));
		if (code == TCL_ERROR) {
			logfmt(LOG_ERROR, "(tcl): ERROR in script(%s): %s\n", script, result);
		    	return TCL_ERROR;
	    	}
		return TCL_OK;
	}
	return TCL_ERROR;
}

/* Unload a script - call unload then delete the interp. */
int tcl_unload_script(char *script) {
	if (!tcl_valid_script(script)) return TCL_ERROR;
	Tcl_Interp *interp = tcl_find_interp_by_script(script);
	int code;
	char *result;
    code = Tcl_Eval(interp, "unload");
    /* Retrieve the result... */
    result = Tcl_GetString(Tcl_GetObjResult(interp));
    /* Check for error! If an error, message is result. */
    if (code == TCL_ERROR) {
        logfmt(LOG_ERROR, "(tcl): ERROR in script(%s): %s\n", script, result);
        return TCL_ERROR;
    }
    jtableP_unset(&tcl_interps, interp);
    jtableS_remove(&tcl_interps_by_script, script);
    Tcl_DeleteInterp(interp);
    if (Tcl_InterpDeleted(interp)) {
    	return TCL_OK;
    }
    return TCL_ERROR;
}

/* Find an interpreter based on the script. */
Tcl_Interp *tcl_find_interp_by_script(char *script) {
	return jtableS_get(&tcl_interps_by_script, script);
}

/* Returns 1 if the script is loaded, 0 if not */
int tcl_valid_script(const char *script) {
	if (jtableS_get(&tcl_interps_by_script, script) != NULL) {
		return 1;
	}
	return 0;
}

/* Run a TCL command in an interpreter */
int tcl_run_command(Tcl_Interp *interp, char *command) {
	return Tcl_Invoke(interp, command);
}

/* Load a script into an interp */
int tcl_insert_script(Tcl_Interp *interp, char *fileName) {
	return Tcl_EvalFile(interp, fileName);
}

int tcl_local_client_create(ClientData cd, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
	if (objc != 8) {
		Tcl_WrongNumArgs(interp, 1, objv, "nickname ident hostname usermodes opername authname realname");
		return TCL_ERROR;
	}
	const char *nickname = Tcl_GetStringFromObj(objv[1], NULL);
	const char *ident = Tcl_GetStringFromObj(objv[2], NULL);
	const char *hostname = Tcl_GetStringFromObj(objv[3], NULL);
	const char *usermodes = Tcl_GetStringFromObj(objv[4], NULL);
	const char *opername = Tcl_GetStringFromObj(objv[5], NULL);
	const char *authname = Tcl_GetStringFromObj(objv[6], NULL);
	const char *realname = Tcl_GetStringFromObj(objv[7], NULL);

	struct user *u = module_create_client(nickname, ident, hostname, usermodes, authname, opername, realname);
	Tcl_SetObjResult(interp, Tcl_NewStringObj(u->numericstr, -1));
	return TCL_OK;
}

int tcl_local_client_destroy(ClientData cd, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
	if (objc != 2) {
		Tcl_WrongNumArgs(interp, 1, objv, "numeric");
		return TCL_ERROR;
	}
	const char *numeric = Tcl_GetStringFromObj(objv[1], NULL);
	module_destroy_client(get_user_by_numericstr(numeric), "Script unloaded.");
	Tcl_ResetResult(interp);
	return TCL_OK;
}

int tcl_local_client_join(ClientData cd, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
	if (objc != 3 && objc != 4) {
		Tcl_WrongNumArgs(interp, 1, objv, "numeric channel ?autoop?");
		return TCL_ERROR;
	}
	const char *numeric = Tcl_GetStringFromObj(objv[1], NULL);
	const char *channel = Tcl_GetStringFromObj(objv[2], NULL);
	int autoop = 0;
	if (objc == 4 && Tcl_GetBooleanFromObj(interp, objv[3], &autoop) != TCL_OK)
		return TCL_ERROR;
	struct user *u = get_user_by_numericstr(numeric);
	module_join_channel(u, channel, autoop);
	Tcl_ResetResult(interp);
	return 0;
}

int tcl_local_client_part(ClientData cd, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
	if (objc != 3) {
		Tcl_WrongNumArgs(interp, 1, objv, "numeric channel");
		return TCL_ERROR;
	}
	const char *numeric = Tcl_GetStringFromObj(objv[1], NULL);
	const char *channel = Tcl_GetStringFromObj(objv[2], NULL);
	struct user *u = get_user_by_numericstr(numeric);
	module_part_channel(u, channel);
	Tcl_ResetResult(interp);
	return 0;
}

int tcl_local_client_privmsg(ClientData cd, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
	if (objc != 4) {
		Tcl_WrongNumArgs(interp, 1, objv, "numeric target message");
		return TCL_ERROR;
	}
	const char *numeric = Tcl_GetStringFromObj(objv[1], NULL);
	const char *target = Tcl_GetStringFromObj(objv[2], NULL);
	const char *message = Tcl_GetStringFromObj(objv[3], NULL);
	struct user *u = get_user_by_numericstr(numeric);
	module_privmsg(u, target, message);
	Tcl_ResetResult(interp);
	return 0;
}

int tcl_local_client_notice(ClientData cd, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]) {
	if (objc != 4) {
		Tcl_WrongNumArgs(interp, 1, objv, "numeric target message");
		return TCL_ERROR;
	}
	const char *numeric = Tcl_GetStringFromObj(objv[1], NULL);
	const char *target = Tcl_GetStringFromObj(objv[2], NULL);
	const char *message = Tcl_GetStringFromObj(objv[3], NULL);
	struct user *u = get_user_by_numericstr(numeric);
	module_notice(u, target, message);
	Tcl_ResetResult(interp);
	return 0;
}

void tcl_init_commands(Tcl_Interp *interp) {
	Tcl_CreateObjCommand(interp, "client_create", tcl_local_client_create, (ClientData)NULL, NULL);
	Tcl_CreateObjCommand(interp, "client_destroy", tcl_local_client_destroy, (ClientData)NULL, NULL);
	Tcl_CreateObjCommand(interp, "client_join", tcl_local_client_join, (ClientData)NULL, NULL);
	Tcl_CreateObjCommand(interp, "client_part", tcl_local_client_part, (ClientData)NULL, NULL);

	Tcl_CreateObjCommand(interp, "client_privmsg", tcl_local_client_privmsg, (ClientData)NULL, NULL);
	Tcl_CreateObjCommand(interp, "client_notice", tcl_local_client_notice, (ClientData)NULL, NULL);
}

static int tcl_evalstr(Tcl_Interp *interp, char *cmd) {
	return Tcl_Eval(interp, cmd);
}

void tcl_onprivmsg(struct user *from, struct user *to, char *msg) {
	char cmd[513];
	logfmt(LOG_ERROR, "Sending '%s' from '%s' to tcl module", msg, from->nick);
	snprintf(cmd, sizeof(cmd), "irc_onprivmsg %s %s %s", from->numericstr, to->numericstr, msg);
	jtableP_iterate(&tcl_interps, (jtableP_cb)tcl_evalstr, cmd);
}
