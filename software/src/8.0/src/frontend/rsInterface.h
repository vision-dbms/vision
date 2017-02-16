#ifndef RSINT_H
#define RSINT_H

/***** Shared definitions *****/
#include "rsInterface.d"
#include "buffers.d"
#include "page.d"

/***** the following two defines can be toggled back and forth in order
       to compile the front end with/without the Research System connections
 *****/
#define RSATTACH 1
#define NOTRSATTACH 0

PublicFnDecl void STD_syswarn (char const *msg);
PublicFnDecl void STD_syserr (char const *msg);

PublicFnDecl int STD_execute (
    char const*			name,
    char*			args[],
    int *			fdin,
    int *			fdout,
    int				(*preChildProcessing)(void)
);
PublicFnDecl int RS_fprintf (char const *fmt, ...);
PublicFnDecl void RS_writeLine (char const *str);
PublicFnDecl void RS_nodelayOutput (int on);
PublicFnDecl int RS_readLine (char *str, int max);
PublicFnDecl void RS_system (char const *str);
PublicFnDecl void RS_dumpOutput ();
PublicFnDecl void RS_readOutput (
    LINEBUFFER *		outbuffer,
    LINEBUFFER *		errbuffer
);
PublicFnDecl void RS_compile ();
PublicFnDecl void RS_save ();
PublicFnDecl int RS_sendAndCheck(char const *input, char const *output);
PublicFnDecl void RS_sendLine(char const *input);
PublicFnDecl void RS_sendOnly (char const *string);

PublicFnDecl int RS_getValue (char *value);
PublicFnDecl int RS_printOutput (PAGE *page);


PublicVarDecl int RS_userSystem; 
PublicVarDecl int RS_AdministratorPrivate; 
PublicVarDecl int RS_DataWasUpdated; 
PublicVarDecl int RS_autog;
PublicVarDecl jmp_buf RS_userjmpbuf;
PublicVarDecl int RS_jmpToUser;
PublicVarDecl char RS_CompanyName[];
PublicVarDecl struct tm *RS_Date;

PublicVarDecl int DEBUG;

#endif
