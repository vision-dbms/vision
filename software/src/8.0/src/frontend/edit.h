#ifndef EDIT_H
#define EDIT_H

#include "buffers.h"

PublicFnDecl int EDIT_reportFileMenu (
     PAGE *page, int doBrowse
);

PublicFnDecl void EDIT_main();
PublicFnDecl void EDIT_initEditor();
PublicFnDecl void EDIT_runInterface(PAGE *iPage);
PublicFnDecl void EDIT_displayErrorMessage();

PublicFnDecl void ED_unsetMarkers(LINEBUFFER *buffer);

PublicFnDecl void ED_subShell();
PublicFnDecl void EDIT_browserIO();

PublicFnDecl int printFile (char const *fname);

PublicVarDecl LINEBUFFER *BrowserBuf, *BrowserInput;

#endif
