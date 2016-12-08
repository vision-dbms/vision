#ifndef PRINT_H
#define PRINT_H

/***** Shared Definitions *****/
#include "print.d"

/***** Variable Declarations *****/
PublicVarDecl int PRINT_Lines;
PublicVarDecl PRINTER_Command PRINT_Command;
PublicVarDecl MENU_Choice *printerChoices;

/***** Function Declarations *****/
PublicFnDecl char *print ();
PublicFnDecl void cleanupPrinters ();
PublicFnDecl int checkPrinters ();

#endif
