/***** IO Handler *****/
#ifndef IO_H
#define IO_H

/******************************
 ***** Public IO Routines *****
 ******************************/

/*****  File Echoing  *****/
PublicFnDecl void IO_EchoToFile (char const *filename);

PublicFnDecl void IO_StopEchoToFile ();

/***** Output Routines *****/
PublicFnDecl int IO_puts (
    char const			*str
);

PublicFnDecl int __cdecl IO_printf (
    char const			*format,
    ...
);

PublicFnDecl int __cdecl IO_fprintf (
    FILE			*file,
    char const			*format,
    ...
);

PublicFnDecl int IO_vnprintf (
    size_t			size,
    char const			*format,
    va_list			ap
);

PublicFnDecl int IO_vfprintf (
    FILE			*file,
    char const			*format,
    va_list			ap
);
    
/***** Prompted Input Routines *****/
PublicFnDecl char *IO_pfgets (
    char const			*prompt,
    char			*string,
    size_t			size
);


#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  io.h 1 replace /users/jad
  860421 17:01:50 jad creation

 ************************************************************************/
/************************************************************************
  io.h 2 replace /users/jad/system
  860422 15:13:49 jad added IO_vprintf and IO_vfprintf they are
currently unused.

 ************************************************************************/
/************************************************************************
  io.h 3 replace /users/jad/system
  860425 12:02:52 jad added two prompting input routines

 ************************************************************************/
/************************************************************************
  io.h 4 replace /users/ees/system
  860528 18:12:41 ees modified input routine to accept a file name: ees 5-28-86

 ************************************************************************/
/************************************************************************
  io.h 5 replace /users/mjc/translation
  870524 09:39:07 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  io.h 6 replace /users/jad/system
  870921 10:44:56 jad implemented echoing to an output file

 ************************************************************************/
/************************************************************************
  io.h 7 replace /users/m2/backend
  890503 15:22:43 m2 Added IO_{f}puts()

 ************************************************************************/
/************************************************************************
  io.h 8 replace /users/m2/backend
  890828 17:04:17 m2 Added command '?> file' for compiling and redirecting to a file.

 ************************************************************************/
