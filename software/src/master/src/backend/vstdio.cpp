/*****  IO Handler  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName IO

/******************************************
 *****  Header and Declaration Files  *****
 ******************************************/

/*****  System  *****/
#include "Vk.h"

/*****  Facility  *****/
#include "batchvision.h"

#include "verr.h"
#include "vfac.h"
#include "vutil.h"

#include "IOMHandle.h"

/*****  Shared  *****/
#include "vstdio.h"


/*****************************
 *****  Output Routines  *****
 *****************************/

PublicFnDef void IO_EchoToFile (char const *filename) {
    IOMDriver *const pChannel = Batchvision::ActiveChannel ();

    if (pChannel)
	pChannel->OpenOutputFile (filename, false);
}

PublicFnDef void IO_StopEchoToFile () {
    IOMDriver *const pChannel = Batchvision::ActiveChannel ();

    if (pChannel)
	pChannel->CloseOutputFile ();
}

PublicFnDef int IO_puts (char const *str) {
    IOMDriver *const pChannel = Batchvision::ActiveChannel ();

    if (pChannel)
	return pChannel->PutString (str);

    int const result = fputs (str, stdout);
    fflush (stdout);
    return result;
}

PublicFnDef int __cdecl IO_printf (char const *format, ...) {
    V_VARGLIST (ap, format);
    return IO_vnprintf (0, format, ap);
}

PublicFnDef int __cdecl IO_fprintf (FILE *file, char const *format, ...) {
    V_VARGLIST (ap, format);
    return IO_vfprintf (file, format, ap);
}

PublicFnDef int IO_vnprintf (size_t size, char const *format, va_list arglist) {
    IOMDriver *const pChannel = Batchvision::ActiveChannel ();

    if (pChannel)
	return pChannel->VPrint (size, format, arglist);
    
    int const result = STD_vprintf (format, arglist);
    fflush (stdout);
    return result;
}

PublicFnDef int IO_vfprintf (FILE *file, char const *format, va_list ap) {
    IOMDriver *const pChannel = Batchvision::ActiveChannel ();

    if (pChannel) {
	if ((FILE*)stdout == file)
	    return pChannel->VPrint (0, format, ap);
	if ((FILE*)stderr == file)
	    return pChannel->VReport (0, format, ap);
    }

    int const result = STD_vfprintf (file, format, ap);
    fflush (file);
    return result;
}


/*************************************
 *****  Prompted Input Routines  *****
 *************************************/

PublicFnDef char *IO_pfgets (char const* prompt, char *pBuffer, size_t sBuffer) {
    IOMDriver *const pChannel = Batchvision::ActiveChannel ();

    IOMHandle handle;
    handle.construct (pChannel);

    bool bNotDone = true;
    do {
	char * pLine;
	size_t sLine;
	switch (pChannel->GetLine (&handle, prompt, &pLine, &sLine)) {
	case VkStatusType_Completed:
	    if (sLine > sBuffer - 1)
		sLine = sBuffer - 1;
	    memcpy (pBuffer, pLine, sLine);
	    pBuffer[sLine] = '\0';
	    UTIL_Free (pLine);
	    bNotDone = false;
	    break;
	case VkStatusType_Blocked:
            if (pChannel)
                pChannel->Wait ();
	    break;
	default:
	    pBuffer = NilOf (char*);
	    bNotDone = false;
	    break;
	}
    } while (bNotDone);
    return pBuffer;
}


/*********************************
 *****  Facility Definition  *****
 *********************************/

FAC_DefineFacility
{
    switch (FAC_RequestTypeField)
    {
    FAC_FDFCase_FacilityIdAsString (IO);
    FAC_FDFCase_FacilityDescription ("IO Handler");
    FAC_FDFCase_FacilitySccsId ("%W%:%G%:%H%:%T%");
    default:
	break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  io.c 1 replace /users/jad
  860421 17:02:04 jad creation

 ************************************************************************/
/************************************************************************
  io.c 2 replace /users/jad/system
  860422 15:13:53 jad added IO_vprintf and IO_vfprintf they are
currently unused.

 ************************************************************************/
/************************************************************************
  io.c 3 replace /users/jad/system
  860425 12:03:00 jad added two prompting input routines

 ************************************************************************/
/************************************************************************
  io.c 4 replace /users/jad/system
  860506 18:13:45 jad made the routines smarter so it doesn't send extra
blank lines

 ************************************************************************/
/************************************************************************
  io.c 5 replace /users/ees/system
  860528 18:12:35 ees modified input routine to accept a file name: ees 5-28-86

 ************************************************************************/
/************************************************************************
  io.c 6 replace /users/jad/system
  860529 14:26:13 jad added a facility declaration

 ************************************************************************/
/************************************************************************
  io.c 7 replace /users/ees/system
  860529 16:11:28 ees changed handshake with Editor to \005\n: ees 5-29-85

 ************************************************************************/
/************************************************************************
  io.c 8 replace /users/mjc/translation
  870524 09:38:58 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  io.c 9 replace /users/mjc/system
  870527 10:33:51 mjc Eliminated need for non-portable 'InputFile = stdin' compile time initialization

 ************************************************************************/
/************************************************************************
  io.c 10 replace /users/mjc/system
  870614 20:01:53 mjc Add '@' as input file redirection directive

 ************************************************************************/
/************************************************************************
  io.c 11 replace /vision/source/revision/interpreter/alists
  870616 14:44:06 insyte Changed printable 'read-from-file' directive to double ?-mark.

 ************************************************************************/
/************************************************************************
  io.c 12 replace /users/jad/system
  870921 10:44:52 jad implemented echoing to an output file

 ************************************************************************/
/************************************************************************
  io.c 13 replace /users/jad/system
  880329 18:07:34 jad make 'echoToFile' echo stderr in addition to stdout

 ************************************************************************/
/************************************************************************
  io.c 14 replace /users/jad/system
  880711 16:13:04 jad Implemented Fault Remembering and 4 New List Primitives

 ************************************************************************/
/************************************************************************
  io.c 15 replace /users/m2/backend
  890503 15:21:10 m2 Added IO_{f}puts() and STD_printf() changes

 ************************************************************************/
/************************************************************************
  io.c 16 replace /users/m2/backend
  890828 17:03:09 m2 Added command '?> file' for compiling and redirecting to a file.

 ************************************************************************/
