/*****  Signal Handling Exported Declarations  *****/
#ifndef SIGHANDLER_H
#define SIGHANDLER_H

/*****  Function Declarations  *****/
PublicFnDecl void display_error (
    char const*			msg
);

PublicFnDecl void handle_signal (
    int				sig,
    int				code,
    struct sigcontext *		scp
);

PublicFnDecl int set_signals (
    void (*routine)(
	int				sig,
	int				code,
	struct sigcontext		*scp
    )
);
     
#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  gopt.h 1 replace /users/mjc/system
  860226 16:48:38 mjc create

 ************************************************************************/


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  sigHandler.h 1 replace /users/jck/updates
  861103 09:01:42 jck Function decls

 ************************************************************************/
