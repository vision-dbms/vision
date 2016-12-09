#ifndef READER_H
#define READER_H

/********************************
 *****  Buffer Declaration  *****
 ********************************/

#define MAX_BUFFERSIZE	524268

struct DATABUFFER {
    int count;
    unsigned char content[MAX_BUFFERSIZE];
};

#define READER_BufferSize(blockSize) (\
    (MAX_BUFFERSIZE / (blockSize)) * (blockSize)\
)


/*******************
 *****  State  *****
 *******************/

PublicVarDecl int
    READER_WarnedRead,
    READER_WarnedAdjust;


/***********************
 *****  Utilities  *****
 ***********************/

PublicFnDecl double dtb (
    char *			s,
    int				n
);


/************************
 *****  Operations  *****
 ************************/

PublicFnDecl void READER_ConvertToAscii (
    unsigned char *		buffer,
    int				count
);

PublicFnDecl void READER_ConvertToEbcdic (
    unsigned char *		buffer,
    int				count
);

PublicFnDecl void READER_InitSemaphoreStructures ();

PublicFnDecl void READER_CreateSemaphores ();

PublicFnDecl void READER_FatalError(
    char const*			msg
);

PublicFnDecl void READER_SpawnReader (
    int				rsize,
    int				tapes,
    int				bufsize,
    int				maxrecs,
    char const*			filename,
    char const*			tapename,
    int				rewind
);

PublicFnDecl void READER_SpawnReader (
    int				rsize,
    int				tapes,
    int				bufsize,
    int				maxrecs,
    char const*			filename,
    char const*			tapename,
    int				rewind,
    char *			skipPattern
);

PublicFnDecl void READER_CreateBuffers ();

PublicFnDecl void READER_ReturnBufferToReader ();

PublicFnDecl DATABUFFER *READER_GetBufferToSave ();

PublicFnDecl void READER_SignalResponse (
    int				sig,
    int				code,
    struct sigcontext *		scp
);

PublicFnDecl int READER_SegmentCreate (
    char const*			fname,
    unsigned int		mode,
    int				bytes
);

PublicFnDecl int READER_FileCreate (
    char const*			fname,
    unsigned int		mode,
    int				bytes
);

PublicFnDecl void READER_CleanupAndExit ();

#endif


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  reader.h 1 replace /users/ees/dbutil
  861020 14:01:24 ees tape reader declarations

 ************************************************************************/
/************************************************************************
  reader.h 2 replace /users/ees/dbutil
  861113 10:10:11 ees added fatal_error() decl

 ************************************************************************/
/************************************************************************
  reader.h 3 replace /users/m2/dbup
  880426 12:21:16 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  reader.h 4 replace /users/jck/dbupdate
  891009 12:57:15 jck Standardizer updates

 ************************************************************************/
