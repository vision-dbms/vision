#ifndef PRINT_H
#define PRINT_H

/***** Shared Definitions *****/
#include "print.d"

/***** Variable Declarations *****/
PublicVarDecl int PRINT_Lines;
PublicVarDecl PRINTER_Command PRINT_Command;
PublicVarDecl MENU_Choice *printerChoices;

/***** Function Declarations *****/
PublicFnDecl void print (PAGE *currPage, int defaultPrinter);
PublicFnDecl void cleanupPrinters ();
PublicFnDecl int checkPrinters ();

#endif
