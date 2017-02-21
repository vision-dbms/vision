/*********************************************************
 **********		error.h			**********
 ********************************************************/
#ifndef ERROR_H
#define ERROR_H

#include "stdcurses.h"
#include "error.d"

PublicVarDecl CUR_WINDOW *ERR_Window;
PublicVarDecl int ERR_msgDisplayed, RS_TabSpacing;

PublicFnDecl void ERR_paintLineASCII (
    char const *str, int width, CUR_WINDOW *win, int useHilight, int hilight
);
PublicFnDecl void ERR_displayNoCurses(int i);
PublicFnDecl void ERR_displayError(int i);
PublicFnDecl void ERR_displayMsg(int i);
PublicFnDecl void ERR_fatal(char const *msg);
PublicFnDecl void ERR_clearMsg();

PublicFnDecl void ERR_displayStr(char const *str, int error);
PublicFnDecl void ERR_displayStrNoRefresh(char const *str);
PublicFnDecl void ERR_displaySearchStr(char const *str);
PublicFnDecl void ERR_displayStrMsg(char const *str, char const *msg, int BEEP);
PublicFnDecl void ERR_displayPause(char const *str);
PublicFnDecl void ERR_displayMsgPause(int i);

PublicFnDecl int ERR_promptYesNo(char const  *msg);
PublicFnDecl int ERR_promptForString(char const *prompt, char *buffer, int allowHelp);
PublicFnDecl int ERR_promptForRepetition(int *num, CUR_WINDOW *win, int oy, int ox);
PublicFnDecl int ERR_promptForInt(char const *str, int *num);
PublicFnDecl int ERR_promptForChar(char const *prompt, char const *validChars);



#endif
