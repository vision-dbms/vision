#ifndef EDIT_H
#define EDIT_H

#include "buffers.h"

PublicFnDecl int EDIT_main(),
                 EDIT_initEditor(),
                 EDIT_runInterface();

PublicFnDecl void ED_unsetMarkers();

PublicFnDecl int ED_subShell(), EDIT_browserIO();

PublicVarDecl LINEBUFFER *BrowserBuf, *BrowserInput;

#endif
