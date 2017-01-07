#ifndef EDIT_H
#define EDIT_H

#include "buffers.h"

PublicFnDecl int EDIT_reportFileMenu (
     PAGE *page, int doBrowse
);

PublicFnDecl void EDIT_main(void);
PublicFnDecl void EDIT_initEditor(void);
PublicFnDecl void EDIT_runInterface(PAGE *iPage);
PublicFnDecl void EDIT_displayErrorMessage(void);

PublicFnDecl void ED_unsetMarkers();

PublicFnDecl void ED_subShell(void);
PublicFnDecl void EDIT_browserIO(void);

PublicVarDecl LINEBUFFER *BrowserBuf, *BrowserInput;

#endif
