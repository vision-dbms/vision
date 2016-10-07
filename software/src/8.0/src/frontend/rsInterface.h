#ifndef RSINT_H
#define RSINT_H

/***** Shared definitions *****/
#include "rsInterface.d"

/***** the following two defines can be toggled back and forth in order
       to compile the front end with/without the Research System connections
 *****/
#define RSATTACH 1
#define NOTRSATTACH 0

#if RSATTACH 
PublicVarDecl int RS_userSystem; 
PublicVarDecl int RS_AdministratorPrivate; 
PublicVarDecl int RS_DataWasUpdated; 
PublicVarDecl int RS_autog;
PublicVarDecl jmp_buf RS_userjmpbuf;
PublicVarDecl int RS_jmpToUser;
PublicVarDecl char RS_CompanyName[];
PublicVarDecl struct tm *RS_Date;
PublicFnDecl void RS_nodelayOutput(), 
		  RS_readOutput(),
		  RS_sendLine(), 
		  RS_sendOnly(), 
		  RS_system(),
		  RS_dumpOutput ();

PublicFnDecl int RS_sendAndCheck();
#endif
  
PublicVarDecl int DEBUG;

#endif
