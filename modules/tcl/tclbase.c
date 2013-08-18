#include <tcl.h>
#include "main.h"
#include "tclbase.h"

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
	return 0;
}

void tcl_init_commands(Tcl_Interp *interp) {
	Tcl_CreateObjCommand(interp, "localclient_create", tcl_local_client_create, (ClientData)NULL, NULL);
	Tcl_CreateObjCommand(interp, "localclient_destroy", tcl_local_client_destroy, (ClientData)NULL, NULL);
	Tcl_CreateObjCommand(interp, "localclient_join", tcl_local_client_destroy, (ClientData)NULL, NULL);
}
