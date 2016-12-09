/*********************************************************
 **********		error.h			**********
 ********************************************************/
#ifndef ERROR_H
#define ERROR_H

#include "stdcurses.h"
#include "error.d"

PublicVarDecl CUR_WINDOW *ERR_Window;
PublicVarDecl int ERR_msgDisplayed, RS_TabSpacing;

PublicFnDecl int ERR_promptYesNo();
PublicFnDecl int ERR_promptForString();
PublicFnDecl int ERR_promptForRepetition();
PublicFnDecl int ERR_promptForInt();
PublicFnDecl int ERR_promptForChar();
PublicFnDecl int ERR_paintLineASCII();

PublicFnDecl void ERR_displayError(), 
		  ERR_displayMsg(),
		  ERR_fatal(), 
		  ERR_displayStr(), 
		  ERR_displayStrNoRefresh(), 
		  ERR_displaySearchStr(),
		  ERR_displayStrMsg(),
		  ERR_displayPause(), 
		  ERR_displayMsgPause(), 
		  ERR_clearMsg();

#endif
