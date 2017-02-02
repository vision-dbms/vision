/****************************************************************/
/*  LOADTAPE                                                    */
/*  Read in file from tape.                                     */
/*  Usage:                                   default            */
/*            loadtape -p allocsize              0              */
/*                     -b block buffer size   10000             */
/*                     -n maxrecords        999999999           */
/*                     -t tape drive        /dev/rmt            */
/*                     -a                       OFF             */
/*                     -N numtapes               1              */
/*                     filename                NULL             */
/*                                                              */
/*  -p, -n, -t, -a, -N  are optional                            */
/*  if "numtapes" is zero then the "tape drive" is a filename   */
/*  -a signifies tape is in ascii                               */
/****************************************************************/

/*************************
 *************************
 *****  Interfaces  ******
 *************************
 *************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

/************************
 *****  Supporting  *****
 ************************/

#include "gopt.h"
#include "reader.h"
#include "sigHandler.h"


/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

PrivateFnDef void DisplayStatus (char const *msg) {
    printf ("%s", msg);
    fflush (stdout);
}


/******************************************
 *  Consumer Process  (Convert and Save)  *
 ******************************************/

PrivateFnDef void convert_and_save (
    char const*			fname,
    int				allocsize,
    int				inascii
)
{
    int ofd;
    DATABUFFER *buffer;

    if ((ofd = READER_FileCreate (fname, 0640, allocsize)) < 0)
	READER_FatalError ("File Create");

    while (buffer = READER_GetBufferToSave ()) {
        if ( inascii == false )
	    READER_ConvertToAscii (buffer->content, buffer->count);

	if (write (ofd, buffer->content, buffer->count) < 0)
	    READER_FatalError ("Write");

	READER_ReturnBufferToReader ();
    }

    close (ofd);
}


/****************************
 *  Main Program Utilities  *
 ****************************/

GOPT_BeginOptionDescriptions
    GOPT_ValueOptionDescription
	("OPTION_N", 'n', NilOf (char *), "999999999")
    GOPT_ValueOptionDescription
	("OPTION_T", 't', "DBSourceName", "/dev/rmt")
    GOPT_ValueOptionDescription
        ("OPTION_P", 'p', NilOf (char *), "0")
    GOPT_SwitchOptionDescription	    
        ("OPTION_A", 'a', NilOf (char *))
    GOPT_ValueOptionDescription
	("OPTION_NT", 'N', NilOf (char *), "1")
    GOPT_ValueOptionDescription
        ("OPTION_B", 'b', NilOf (char *), "10000")
GOPT_EndOptionDescriptions;

PrivateFnDef void collect_arguments (
    int				argc,
    char*			argv[],
    char const**		fname,
    char const**		tname,
    int*			allocsize,
    int*			maxrecs,
    int*			inascii,
    int*			numtapes,
    int*			readsize
)
{
    GOPT_AcquireOptions(argc, argv);

    *tname	= GOPT_GetValueOption("OPTION_T");
    *allocsize	= atoi(GOPT_GetValueOption("OPTION_P"));
    *maxrecs	= atoi(GOPT_GetValueOption("OPTION_N"));
    *inascii	= GOPT_GetSwitchOption("OPTION_A");
    *numtapes	= atoi(GOPT_GetValueOption("OPTION_NT"));
    *readsize   = atoi(GOPT_GetValueOption("OPTION_B"));

    GOPT_SeekExtraArgument(0, 0);
    *fname = GOPT_GetExtraArgument();

    if (*fname == NULL)
    {
	fprintf (stderr, "No Output File Specified\n");
        fprintf (stderr, "Usage: loadtape [-p -b -n -t -a -N] filename\n");
	exit (ErrorExitValue);
    }
}


/******************
 *  Main Program  *
 ******************/

int main (
    int				argc,
    char*			argv[]
)
{
    char const *fname, *tname;
    int allocsize, maxrecs, inascii, numtapes, readsize;
    extern int SignalResponse();

    READER_InitSemaphoreStructures ();

    collect_arguments (argc, argv, &fname, &tname, &allocsize, &maxrecs, &inascii, &numtapes, &readsize);

    set_signals (READER_SignalResponse);
    READER_CreateBuffers ();
    READER_CreateSemaphores ();

    DisplayStatus ("Begin Processing>>>");
    READER_WarnedRead = READER_WarnedAdjust = false;
    READER_SpawnReader
         (readsize, 
	  numtapes, 
	  MAX_BUFFERSIZE,
	  maxrecs,
	  fname,
	  tname,
	  numtapes > 1);
    convert_and_save (fname, allocsize, inascii);
    DisplayStatus ("<<<End Processing\n");

    READER_CleanupAndExit();

    return NormalExitValue;
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  loadtape.c 1 replace /users/mjc/util
  870623 09:28:58 mjc Release tape loader as a system utility

 ************************************************************************/
/************************************************************************
  loadtape.c 2 replace /users/mjc/TRowe/helpers/src
  870625 13:28:27 mjc Changed status messages to be more meaningful

 ************************************************************************/
/************************************************************************
  loadtape.c 3 replace /users/lis/updates/tape
  870821 15:51:11 lis conversion to ascii made optional

 ************************************************************************/
/************************************************************************
  loadtape.c 4 replace /users/jck/util
  871009 13:49:50 jck Fixed bug in the handling of the death of the reader

 ************************************************************************/
/************************************************************************
  loadtape.c 5 replace /users/jck/tools
  880503 09:09:20 jck Changes required by Port to Apollo

 ************************************************************************/
/************************************************************************
  loadtape.c 6 replace /users/jck/tools
  880510 21:55:02 jck Replaced inadvertently removed code implementing EBCDIC -> ASCII translation

 ************************************************************************/
/************************************************************************
  loadtape.c 7 replace /users/jck/tools
  880512 15:59:02 jck Added gopt facilities that check for DBSourceName env variable to loadtape

 ************************************************************************/
/************************************************************************
  loadtape.c 8 replace /users/jck/dbupdate
  891009 13:06:55 jck Moving from tools to dbupdate so that loadtape can share the reader module

 ************************************************************************/
/************************************************************************
  loadtape.c 9 replace ~src_con/Vision/dbupdate/dbupdate_lib/dbupdate
  911122 10:03:55 teg Add aoption b to set buffer size for reading a block

 ************************************************************************/
