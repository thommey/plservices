#ifndef TCLBASE_H_
#define TCLBASE_H_

/* Commands */
int tcl_register_local_client(void *clientData, Tcl_Interp *interp, int argc, const char **argv);
int tcl_quit_local_client(void *clientData, Tcl_Interp *interp, int argc, const char **argv);
void tcl_init_commands(Tcl_Interp *interp);

#endif
