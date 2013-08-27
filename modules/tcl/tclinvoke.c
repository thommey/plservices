#if defined(__STDC__) || defined(HAS_STDARG)
#   include <stdarg.h>
#else
#   include <varargs.h>
#endif

int Tcl_Invoke TCL_VARARGS_DEF(Tcl_Interp *, arg1)
{
   va_list argList;
   Tcl_Interp *interp;
   char *cmd;          /* Command name */
   char *arg;          /* Command argument */
   char **argv;        /* String vector for arguments */
   int argc, i, max;   /* Number of arguments */
   Tcl_CmdInfo info;   /* Info about command procedures */
   int result;         /* TCL_OK or TCL_ERROR */

   interp = TCL_VARARGS_START(Tcl_Interp *, arg1, argList);
   Tcl_ResetResult(interp);

   /*
    * Map from the command name to a C procedure
    */
   cmd = va_arg(argList, char *);
   if (! Tcl_GetCommandInfo(interp, cmd, &info)) {
      Tcl_AppendResult(interp, "unknown command \"",
          cmd, "\"", NULL);
      va_end(argList);
      return TCL_ERROR;
   }

   max = 20;           /* Initial size of argument vector */

#if TCL_MAJOR_VERSION > 7
   /*
    * Check whether the object interface is preferred for
    * this command
    */

   if (info.isNativeObjectProc) {
      Tcl_Obj **objv;     /* Object vector for arguments */
      Tcl_Obj *resultPtr; /* The result object */
      int objc;

      objv = (Tcl_Obj **) Tcl_Alloc(max * sizeof(Tcl_Obj *));
      objv[0] = Tcl_NewStringObj(cmd, strlen(cmd));
      Tcl_IncrRefCount(objv[0]); /* ref count == 1*/
      objc = 1;

      /*
       * Build a vector out of the rest of the arguments
       */

      while (1) {
         arg = va_arg(argList, char *);
         if (arg == (char *)NULL) {
            objv[objc] = (Tcl_Obj *)NULL;
            break;
         }
         objv[objc] = Tcl_NewStringObj(arg, strlen(arg));
         Tcl_IncrRefCount(objv[objc]); /* ref count == 1*/
         objc++;
         if (objc >= max) {
            /* allocate a bigger vector and copy old one */
            Tcl_Obj **oldv = objv;
            max *= 2;
            objv = (Tcl_Obj **) Tcl_Alloc(max *
                   sizeof(Tcl_Obj *));
            for (i = 0 ; i < objc ; i++) {
               objv[i] = oldv[i];
            }
            Tcl_Free((char *)oldv);
         }
      }
      va_end(argList);

      /*
       * Invoke the C procedure
       */
      result = (*info.objProc)(info.objClientData, interp,
             objc, objv);

      /*
       * Make sure the string value of the result is valid
       * and release our references to the arguments
       */
      (void) Tcl_GetStringResult(interp);
      for (i = 0 ; i < objc ; i++) {
         Tcl_DecrRefCount(objv[i]);
      }
      Tcl_Free((char *)objv);

      return result;
   }
#endif
    argv = (char **) Tcl_Alloc(max * sizeof(char *));
    argv[0] = cmd;
    argc = 1;

       /*
        * Build a vector out of the rest of the arguments
        */
       while (1) {
          arg = va_arg(argList, char *);
          argv[argc] = arg;
          if (arg == (char *)NULL) {
             break;
          }
          argc++;
          if (argc >= max) {
             /* allocate a bigger vector and copy old one */
             char **oldv = argv;
             max *= 2;
             argv = (char **) Tcl_Alloc(max * sizeof(char *));
             for (i = 0 ; i < argc ; i++) {
                argv[i] = oldv[i];
             }
             Tcl_Free((char *) oldv);
          }
       }
       va_end(argList);

       /*
        * Invoke the C procedure
        */
        result = (*info.proc)(info.clientData, interp, argc, argv);

       /*
        * Release the arguments
        */
       Tcl_Free((char *) argv);
       return result;

}
