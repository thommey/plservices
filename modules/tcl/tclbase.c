#include <tcl.h>
#include "main.h"
#include "tclbase.h"

int tcl_local_client_create(void *clientData, Tcl_Interp *interp, int argc, const char **argv)
{
	if (argc < 7) { 
        	Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0]," nickname ident hostname usermodes opername authname realname\"", (char *) NULL);
        	return TCL_ERROR;
	}
	char *nickname = argv[1];
	char *ident = argv[2];
	char *hostname = argv[3];
	char *usermodes = argv[4];
	char *opername = argv[6];
	char *authname = argv[5];
	char *realname = argv[7];
	//struct user *module_create_client(char *nick, const char *ident, const char *hostname, char *modes, char *account, char *opername, const char *realname)
	struct user *u = module_create_client(nickname, ident, hostname, usermodes, authname, opername, realname);
	Tcl_SetResult (interp, u->numericstr, TCL_DYNAMIC);
    	return TCL_OK;
}

int tcl_local_client_destroy(void *clientData, Tcl_Interp *interp, int argc, const char **argv) { 
	if (argc != 2) { 
                Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0]," numeric\"", (char *) NULL);
                return TCL_ERROR;
	}
	char *numeric = argv[1];
	module_destroy_client(get_user_by_numericstr(numeric), "Script unloaded.");
	Tcl_SetResult (interp, NULL, TCL_DYNAMIC);
	return TCL_OK;
}

int tcl_local_client_join(void *clientData, Tcl_Interp *interp, int argc, const char **argv) {
	if (argc != 3) { 
                Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0]," numeric channel\"", (char *) NULL);
                return TCL_ERROR;
	}
	char *numeric = argv[1];
	char *channel = argv[2];
	int autoop = (int)&argv[3];
	struct user *u = get_user_by_numericstr(numeric);
	module_join_channel(u, channel, autoop);
	return 0;
}

void tcl_init_commands(Tcl_Interp *interp) { 
	Tcl_CreateCommand (interp, "localclient_create", tcl_local_client_create, (ClientData)NULL, (void (*)())NULL);
	Tcl_CreateCommand (interp, "localclient_destroy", tcl_local_client_destroy, (ClientData)NULL, (void (*)())NULL);
	Tcl_CreateCommand (interp, "localclient_join", tcl_local_client_destroy, (ClientData)NULL, (void (*)())NULL);
}
