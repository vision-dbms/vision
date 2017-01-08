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

PublicFnDecl void STD_syswarn (char *msg);
PublicFnDecl void STD_syserr (char *msg);
PublicFnDecl int STD_execute (
    char *			name,
    char *			args[],
    int *			fdin,
    int *			fdout,
    int				(*preChildProcessing)(void)
);
PublicFnDecl int RS_fprintf (char *fmt, ...);
PublicFnDecl void RS_writeLine (char *str);
PublicFnDecl void RS_nodelayOutput (int on);
PublicFnDecl int RS_readLine (char *str, int max);
PublicFnDecl void RS_system (char *str);
PublicFnDecl void RS_dumpOutput (void);
PublicFnDecl void RS_readOutput (
    LINEBUFFER *		outbuffer,
    LINEBUFFER *		errbuffer
);
PublicFnDecl void RS_compile (void);
PublicFnDecl void RS_save (void);
PublicFnDecl int RS_sendAndCheck(char *input, char *output);
PublicFnDecl void RS_sendLine(char *input);
PublicFnDecl void RS_sendOnly (char *string);

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
