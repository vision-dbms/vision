/*****************************************
 **********	reader.c	**********
 *****************************************/

/*************************************************
 *************************************************
 *****  Interfaces and Forward Declarations  *****
 *************************************************
 *************************************************/

#include "Vk.h"
#include "VkMemory.h"

#include "reader.h"
#include "sigHandler.h"

PrivateFnDef void FillBuffer (
    DATABUFFER*			pBuffer,
    int				rsize,		/*** record or block size	***/
    int				tapes,		/*** number of tapes		***/
    int				bufsize,	/*** buffersize			***/
    int				maxrecs,	/*** max recs to read		***/
    char const			*filename,	/*** database name		***/
    char const			*tapename,	/*** input name (/dev/rmt)	***/
    int				rewind		/*** do we rewind this tape	***/
);

PrivateFnDef void PassBufferToSaver (
    int				incrementingProductionCount
);


/******************************************
 ******************************************
 *****  Internal Buffer Declarations  *****
 ******************************************
 ******************************************/

#if defined(_WIN32)

#define BUFFERCOUNT	1

PrivateVarDef int	RSIZE, TAPES, BUFSIZE, MAXRECS, REWIND;
PrivateVarDef char*	FILENAME;
PrivateVarDef char*	TAPENAME;

#else

#define BUFFERCOUNT	2

#endif

/*****  Reader variables  *****/
PrivateVarDef int
    InputFD,			/*** tape fildes             ***/
    Tapefile,
    NRecs		= 0,	/*** total records read      ***/
    ReadByteCnt		= 1,	/*** bytes read per read     ***/
    TapeNum		= 1,	/*** current tape number     ***/
    TapeRecsRead	= 0;	/*** records read per tape   ***/

PrivateVarDef char
    Answer[80];

PrivateVarDef char
    *SkipPattern	= NilOf (char *);


/******************************
 ******************************
 *****  Utility Routines  *****
 ******************************
 ******************************/

/****************************************
 *****  EBCDIC -> ASCII Conversion  *****
 ****************************************/

/**********************
 *  Conversion Table  *
 **********************/

PrivateVarDef char EBCDIC_To_ASCII_CTable [256] = {
    '\000', '\001', '\002', '\003', '\234', '\011', '\206', '\177',
    '\227', '\215', '\216', '\013', '\014', '\015', '\016', '\017',
    '\020', '\021', '\022', '\023', '\235', '\205', '\010', '\207',
    '\030', '\031', '\222', '\217', '\034', '\035', '\036', '\037',
    '\200', '\201', '\202', '\203', '\204', '\012', '\027', '\033',
    '\210', '\211', '\212', '\213', '\214', '\005', '\006', '\007',
    '\220', '\221', '\026', '\223', '\224', '\225', '\226', '\004',
    '\230', '\231', '\232', '\233', '\024', '\025', '\236', '\032',
    '\040', '\240', '\241', '\242', '\243', '\244', '\245', '\246',
    '\247', '\250', '\325', '\056', '\074', '\050', '\053', '\174',
    '\046', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
    '\260', '\261', '\041', '\044', '\052', '\051', '\073', '\176',
    '\055', '\057', '\262', '\263', '\264', '\265', '\266', '\267',
    '\270', '\271', '\313', '\054', '\045', '\137', '\076', '\077',
    '\272', '\273', '\274', '\275', '\276', '\277', '\300', '\301',
    '\302', '\140', '\072', '\043', '\100', '\047', '\075', '\042',
    '\303', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
    '\150', '\151', '\304', '\305', '\306', '\307', '\310', '\311',
    '\312', '\152', '\153', '\154', '\155', '\156', '\157', '\160',
    '\161', '\162', '\136', '\314', '\315', '\316', '\317', '\320',
    '\321', '\345', '\163', '\164', '\165', '\166', '\167', '\170',
    '\171', '\172', '\322', '\323', '\324', '\133', '\326', '\327',
    '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
    '\340', '\341', '\342', '\343', '\344', '\135', '\346', '\347',
    '\173', '\101', '\102', '\103', '\104', '\105', '\106', '\107',
    '\110', '\111', '\350', '\351', '\352', '\353', '\354', '\355',
    '\175', '\112', '\113', '\114', '\115', '\116', '\117', '\120',
    '\121', '\122', '\356', '\357', '\360', '\361', '\362', '\363',
    '\134', '\237', '\123', '\124', '\125', '\126', '\127', '\130',
    '\131', '\132', '\364', '\365', '\366', '\367', '\370', '\371',
    '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
    '\070', '\071', '\372', '\373', '\374', '\375', '\376', '\377'
};
PrivateVarDef unsigned char ASCII_To_EBCDIC_CTable [256];

PrivateFnDef void Generate_ASCII_to_EBCDIC ()
{
    unsigned char i;

    for (
	 i = (unsigned char) 0;
	 i < (unsigned char) 255;
	 i++
    ) ASCII_To_EBCDIC_CTable [(unsigned char) EBCDIC_To_ASCII_CTable [i]] = i;

    ASCII_To_EBCDIC_CTable [(unsigned char) EBCDIC_To_ASCII_CTable [i]] = i;

}


/*************************
 *  Conversion Routines  *
 *************************/

PublicFnDef void READER_ConvertToAscii (
    unsigned char *		buffer,
    int				count
)
{
    unsigned char *		ptr;
    register int		i;

    for (
	i = 0, ptr = buffer;
	i < count ;
	i++  , ptr++
    ) *ptr = EBCDIC_To_ASCII_CTable[*ptr];
}

PublicFnDef void READER_ConvertToEbcdic (
    unsigned char *		buffer,
    int				count
)
{
    unsigned char *		ptr;
    register int		i;
    static bool			ASCII_to_EBCDIC_tableGenerated = false;

    if (!ASCII_to_EBCDIC_tableGenerated)
    {
	Generate_ASCII_to_EBCDIC ();
	ASCII_to_EBCDIC_tableGenerated = true;
    }

    for (
	i = 0, ptr = buffer;
	i < count ;
	i++  , ptr++
    ) *ptr = ASCII_To_EBCDIC_CTable[*ptr];

}


/*****************************************
 *****  String -> Double Conversion  *****
 *****************************************/

PublicFnDef double dtb (
    char *			s,
    int				n
)
{
    char    buf[512];

    memcpy(buf, s, n);
    buf[n] = '\0';
    return atof (buf);
}


/***************************
 *****  Tape Rewinder  *****
 ***************************/

#if defined(__hp9000s800) || defined (__hp9000s700)
PrivateFnDef void RewindTape (char const* tapename) {
    char command[256];

    sprintf (command, "mt -t %s rew", tapename);
    system (command);
}

#elif defined(_AIX) || defined(sun) || defined(__linux__)
PrivateFnDef void RewindTape (char const* tapename) {
    char command[256];

    sprintf (command, "mt -f %s rewind", tapename);
    system (command);
}

#elif defined(_WIN32)
PrivateFnDef void RewindTape (char const* tapename) {
}

#endif


/************************************
 ************************************
 *****  Status Area Management  *****
 ************************************
 ************************************/

PrivateVarDef VkMemory StatusAreaDescriptor;

PrivateVarDef struct StatusArea {
    int			readerTerminated;
    int			readerExitStatus;
    unsigned long	buffersProduced;
    unsigned long	buffersConsumed;
#if !defined(_WIN32)
    struct sembuf	p_op;
    struct sembuf	v_op;
#endif
} *StatusArea = NilOf (struct StatusArea *);

#define ReaderTerminated	(StatusArea->readerTerminated)
#define ReaderExitStatus	(StatusArea->readerExitStatus)
#define BuffersProduced		(StatusArea->buffersProduced)
#define BuffersConsumed		(StatusArea->buffersConsumed)
#define P_op			(&StatusArea->p_op)
#define V_op			(&StatusArea->v_op)

PrivateFnDef void CreateStatusArea () {
    if (IsntNil (StatusArea))
	return;

    if (!StatusAreaDescriptor.Allocate (sizeof (struct StatusArea), true))
	READER_FatalError ("Status Area Allocation");
    StatusArea = (struct StatusArea *)StatusAreaDescriptor.RegionAddress ();

    ReaderTerminated	= false;
    ReaderExitStatus	= ErrorExitValue;
    BuffersProduced	=
    BuffersConsumed	= 0;
}

PrivateFnDef void DeleteStatusArea () {
    if (IsNil (StatusArea))
	return;

    StatusArea = NilOf (struct StatusArea *);
    StatusAreaDescriptor.Destroy ();
}
	
PublicVarDef int		READER_WarnedRead	= true,
				READER_WarnedAdjust	= true;

#if defined(_WIN32)
PrivateVarDef int		ReadingCompleted	= false;
#endif


/**********************************
 **********************************
 *****  Semaphore Management  *****
 **********************************
 **********************************/

PrivateVarDef int		EmptySemaphore	= (-1),
				FullSemaphore	= (-1);
PrivateVarDef pid_t		ReaderPid	= (pid_t)(-1);
    
#define ChildProcess	(ReaderPid == 0)

/*****  Initializtion and Cleanup Utilities  *****/
PublicFnDef void READER_InitSemaphoreStructures (
    void
)
{
    CreateStatusArea ();

#if !defined(_WIN32)
    P_op->sem_num = V_op->sem_num = 0;
    P_op->sem_flg = V_op->sem_flg = 0;

    P_op->sem_op = (-1);
    V_op->sem_op = 1;
#endif
}

#if defined(__hp9000s800) || defined(__hp9000s700) || defined(_WIN32)
PrivateFnDef int SemaphoreValue (
    int				value
)
{
    return value;
}
#else

#if defined(_AIX) || defined(sun)
union semun {
    int val;
    struct semid_ds *id;
    ushort *array;
};
#elif defined(__linux__)
union semun {
    int val;
    struct semid_ds *id;
    ushort *array;
    struct seminfo *__buf;
};
#endif

PrivateFnDef union semun SemaphoreValue (
    int				value
)
{
    union semun arg;

    arg.val = value;
    return arg;
}
#endif

PublicFnDef void READER_CreateSemaphores (
    void
)
{
#if !defined(_WIN32)
    key_t	empty_key   = IPC_PRIVATE,
		full_key    = IPC_PRIVATE;

    int semflg = IPC_CREAT | 0600;

    if ((EmptySemaphore = semget (empty_key, 1, semflg)) < 0)
	READER_FatalError ("Empty Semaphore Creation");

    if ((FullSemaphore = semget (full_key, 1, semflg)) < 0)
	READER_FatalError ("Full Semaphore Creation");

    if (semctl (EmptySemaphore, 0, SETVAL, SemaphoreValue (BUFFERCOUNT)) < 0)
	READER_FatalError ("Empty Semaphore Initialization");

    if (semctl (FullSemaphore, 0, SETVAL, SemaphoreValue (0)) < 0)
	READER_FatalError ("Full Semaphore Initialization");
#endif
}

PrivateFnDef void CleanupSemaphores (
    void
)
{
#if !defined(_WIN32)
    if (ChildProcess)
	return;
    if (EmptySemaphore >= 0)
	semctl (EmptySemaphore, 0, IPC_RMID, SemaphoreValue (0));
    if (FullSemaphore >= 0)
	semctl (FullSemaphore, 0, IPC_RMID, SemaphoreValue (0));
#endif

    DeleteStatusArea ();
}


PrivateFnDef int p (
    int				semaphore
)
{
#if defined(_WIN32)
    return true;
#else
    int result;
    
    for (
	errno = 0;
	(result = semop (semaphore, P_op, 1)) != 0 && errno == EINTR;
    );

    if (result == 0) return true;

    /****  Time to stop processing  ****/
/*    if (errno != EAGAIN && errno != EIDRM)	*/
    if (errno != EAGAIN)
	perror ("P Error");

    return false;
#endif
}

PrivateFnDef int v (
    int				semaphore
)
{
#if defined(_WIN32)
    return true;
#else
    if (semop (semaphore, V_op, 1) == 0)
	return true;
    else
    {
	perror ("V Error");
	return false;
    }
#endif
}


/**********************************************
 **********************************************
 *****  Reader Exit and Error Management  *****
 **********************************************
 **********************************************/

PrivateFnDef void KillReader (
    void
)
{
#if !defined(_WIN32)
    if (ReaderPid > 0 && ReaderTerminated != true)
	kill (ReaderPid, SIGTERM);
#endif
}

PrivateFnDef void ReaderExit (int val) {
    if (ChildProcess)
    {
	ReaderTerminated = true;
	ReaderExitStatus = val;
	PassBufferToSaver (false);	/* Generate an 'End-Of-Input' buffer
					   to force the writer to exit.  */
    }

    exit(val);
}

PublicFnDef void READER_FatalError (char const* msg) {
    display_error(msg);
    KillReader ();
    CleanupSemaphores ();
    ReaderExit (ErrorExitValue);
}


/*****************************
 *****************************
 *****  Prompt Routines  *****
 *****************************
 *****************************/

PrivateVarDef FILE *PromptFile;

PrivateFnDef int OpenPromptFile () {
    PromptFile = fopen ("console", "w");
    if (NULL == PromptFile)
    {
        perror ("Prompt file open");
	return true;
    }
    setbuf (PromptFile, NULL);
    return false;
}

PrivateFnDef int Prompt (char const *fmt, ...) {
    va_list ap;
    int result;
    
    va_start (ap, fmt);

    vprintf (fmt, ap);
    result = vfprintf (PromptFile, fmt, ap);
    va_end(ap);    
    return result;
}

PrivateFnDef int ClosePromptFile () {
    return fclose (PromptFile);
}


/*********************************************
 **********	Buffer Routines	   ***********
 *********************************************/

PrivateVarDef VkMemory BufferDescriptor;

PrivateVarDef DATABUFFER *Buffers = NilOf (DATABUFFER *);

PrivateVarDef int
    NextFreeBuffer	= 0,
    NextFilledBuffer	= 0;

PublicFnDef void READER_CreateBuffers () {
    if (IsntNil (Buffers))
	return;

    if (!BufferDescriptor.Allocate (BUFFERCOUNT * sizeof (DATABUFFER), true))
	READER_FatalError ("Buffer Creation");

    Buffers = (DATABUFFER *)BufferDescriptor.RegionAddress ();
}

PrivateFnDef void DeleteBuffers () {
    if (IsNil (Buffers))
	return;

    Buffers = NilOf (DATABUFFER *);
    BufferDescriptor.Destroy ();
}

PrivateFnDef DATABUFFER *GetBufferToLoad () {
    DATABUFFER *result;

    if (p (EmptySemaphore))
    {
        result = &Buffers [NextFreeBuffer];
	NextFreeBuffer = (NextFreeBuffer + 1) % BUFFERCOUNT;
    }
    else
	result = NULL;

    return result;
}

PrivateFnDef void PassBufferToSaver (
    int				incrementingProductionCount
)
{
    if (incrementingProductionCount) BuffersProduced++;
    v (FullSemaphore);
}


PublicFnDef void READER_ReturnBufferToReader (
    void
)
{
    BuffersConsumed++;
    v (EmptySemaphore);
}


PublicFnDef DATABUFFER *READER_GetBufferToSave (
    void
)
{
    DATABUFFER *result;

#if defined(_WIN32)
    if (ReadingCompleted)
    {
        Prompt("\nRecords read = %d\n", NRecs);
        ClosePromptFile ();
        result = NULL;
    }
    else
    {
	result = &Buffers [NextFilledBuffer];
	NextFilledBuffer = (NextFilledBuffer + 1) % BUFFERCOUNT;
	FillBuffer (result, RSIZE, TAPES, BUFSIZE, MAXRECS, FILENAME, TAPENAME, REWIND);
    }
#else
    if (p (FullSemaphore) && BuffersProduced > BuffersConsumed)
    {
        result = &Buffers [NextFilledBuffer];
	NextFilledBuffer = (NextFilledBuffer + 1) % BUFFERCOUNT;
    }
    else result = NULL;
#endif

    return result;
}


/*********************************************
 ********	Signal Handler	   ***********
 *********************************************/

PublicFnDef void READER_SignalResponse (
    int				sig,
    int				Unused(code),
    struct sigcontext		*scp
)
{
    STD_setRestartAfterSignal(scp);

    switch (sig) {

    case SIGHUP:
	READER_FatalError ("*** hangup signal received ***");
	break;
	
    case SIGINT:
	READER_FatalError ("*** interrupt signal received ***");
	break;
	
    case SIGQUIT:
	READER_FatalError ("*** quit signal received ***");
	break;
	
    case SIGILL:
	READER_FatalError ("*** illegal instruction signal received ***");
	break;
	
    case SIGTRAP:
	READER_FatalError ("*** trace trap signal received ***");
	break;
	
    case SIGIOT:
	READER_FatalError ("*** abort (IOT) signal received ***");
	break;
	
    case SIGEMT:
	READER_FatalError ("*** software generated signal received ***");
	break;
	
    case SIGFPE:
	READER_FatalError ("*** floating point exception signal received ***");
	break;
	
    case SIGBUS:
	READER_FatalError ("*** bus error signal received ***");
	break;
	
    case SIGSEGV:
	READER_FatalError ("*** segmentation violation signal received ***");
	break;
	
    case SIGSYS:
	READER_FatalError ("*** bad arg. to system call signal received ***");
	break;
	
    case SIGPIPE:
	READER_FatalError ("*** write on a pipe with no reader signal received ***");
	break;
	
    case SIGALRM:
	READER_FatalError ("*** alarm clock signal received ***");
	break;
	
    case SIGTERM:
	READER_FatalError ("*** software termination signal received ***");
	break;
	
    case SIGUSR1:
	READER_FatalError ("*** user signal 1 signal received ***");
	break;
	
    case SIGUSR2:
	READER_FatalError ("*** user defined signal 2 signal received ***");
	break;
	
    case SIGPWR:
	READER_FatalError ("*** power fail signal received ***");
	break;
	
    case SIGVTALRM:
	READER_FatalError ("*** virtual timer alarm signal received ***");
	break;
	
    case SIGPROF:
	READER_FatalError ("*** profiling timer alarm signal received ***");
	break;
	
    case SIGIO:
	READER_FatalError ("*** SIGIO signal received ***");
	break;
	
    case SIGWINDOW:
	READER_FatalError ("*** window or mouse signal received ***");
	break;

    case SIGCHLD:
	if (ChildProcess) READER_FatalError (
	    "*** child status has changed ***"
	);
	else if (!ReaderTerminated)
	{
	    ReaderTerminated = true;
	    PassBufferToSaver (false);	/* Generate an 'End-Of-Input' buffer
					   to force the writer to exit.  */
	}
	break;
	
    case SIGTTIN:
        READER_FatalError ("*** background read attempted from control terminal ***");
	break;

    case SIGTTOU:
        READER_FatalError ("*** background write attempted to control terminal ***");
	break;

    case SIGXCPU:
        READER_FatalError ("*** cpu time limit exceeded ***");
	break;
	
    case SIGXFSZ:
        READER_FatalError ("*** file size limit exceeded ***");
	break;
	
    case SIGURG:
        READER_FatalError ("*** urgent condition present on socket ***");
	break;
	
    default:
	READER_FatalError ("*** unknown signal received ***");
	break;
    }

    ReaderExit (ErrorExitValue);
}


/*****************************
 *****  Reader Routines  *****
 *****************************/

PrivateFnDef int OpenInput (
    char const			*tapename,
    char const			*filename,
    int				tapes,
    int				rewind
)
{
    if (tapes > 0 && rewind)	
        Tapefile = true;
    else
	Tapefile = false;


    Answer[0] = '\0';
    while (Tapefile && 0 != strncmp(Answer, "ok", 2))
    {
	Prompt("\nPlease load tape %d of %d for %s, when ready type 'ok' ",
	    TapeNum, tapes, filename);
	scanf("%s", Answer);
	if (0 == strncmp(Answer, "quit", 4))
	    READER_FatalError("Update Aborted");
    }

    bool not_open = true;
    while (not_open)
    {
	if ((InputFD = open (tapename, O_RDONLY, 0)) < 0)
	{
	    if (!Tapefile)
		READER_FatalError(tapename);

	    Answer[0] = '\0';
	    while (0 != strncmp(Answer, "ok", 2))
	    {
	    Prompt("\nTape Drive Open Error. \nPlease check BOT and ONLINE lights on tape drive, when ready type 'ok'");
	    scanf("%s", Answer);
	    }
	}
	else
	    not_open = false;
    }
    if (Tapefile) 
        Prompt ("\n... Reading tape %d of %d ...", TapeNum, tapes);
    return 0;
}


PrivateFnDef void FillBuffer (
    DATABUFFER*			pBuffer,
    int				rsize,		/*** record or block size	***/
    int				tapes,		/*** number of tapes		***/
    int				bufsize,	/*** buffersize			***/
    int				maxrecs,	/*** max recs to read		***/
    char const			*filename,	/*** database name		***/
    char const			*tapename,	/*** input name (/dev/rmt)	***/
    int				rewind		/*** do we rewind this tape	***/
)
{
    int				i, restart;		
    unsigned char		*rp, *rplimit;

    if (rsize >= bufsize)
        {
    	rsize = bufsize - 1;
        if (!READER_WarnedAdjust)
 	   {
           Prompt ("\nRead buffer larger than internal buffer.\n");
           Prompt ("Read buffer size adjusted.\n");
           READER_WarnedAdjust = true;
	   }
        }

    for (rp = pBuffer->content, rplimit = rp + bufsize;
	 rplimit - rp >= rsize && NRecs < maxrecs;)
    {
	if ((ReadByteCnt = read(InputFD, rp, rsize)) > 0)
	{
	    TapeRecsRead++;
	    if (IsntNil (SkipPattern) &&
		ReadByteCnt >= (int) strlen (SkipPattern) &&
		0 == memcmp (rp, SkipPattern, strlen (SkipPattern)))
	    {
		continue;
	    }

	    if ((ReadByteCnt == rsize) && !READER_WarnedRead)
	    {
		Prompt ("\nRead buffer filled when reading tape block.\n");
		Prompt ("Potential for block truncation which could cause data loss.\n");
		Prompt ("Use -b option to increase read buffer size.\n");
		READER_WarnedRead = true;
		Prompt ("Current buffer size = %d\n", rsize);
	    }
	    rp += (ReadByteCnt);
	    NRecs++;
	}
	else if (0 == ReadByteCnt)	/**** EOF ****/
	{
	    close(InputFD);

	    if (Tapefile && rewind)
		RewindTape (tapename);

	    if (TapeNum >= tapes)	/**** we're done ****/
	    {
#if defined(_WIN32)
		ReadingCompleted = true;
#endif
		break;
	    }
	    else
	    {
		TapeNum++;
		TapeRecsRead = 0;
		    
		OpenInput (tapename, filename, tapes, rewind);
	    } /* another tape */
	} /* if EOF */       
	else if (ReadByteCnt < 0){
	    close(InputFD);
	    if (!Tapefile)
		READER_FatalError("File Read Error");

	    restart = true;
	    while (restart)
	    {
		Prompt("Tape Read Error [errinfo = 0, errno = %d]", errno);
		Prompt ("Please Clean tape drive before reloading.\n");
		Prompt ("\t(Use 'quit' to abort update)\n");
		    
		OpenInput (tapename, filename, tapes, rewind);

		
		for (i = 0; i < TapeRecsRead; i++)
		{
		    if ((ReadByteCnt = read(InputFD, rp, rsize)) <= 0)
		    {
			close(InputFD);
			break;
		    }
		}
		    
		if (i == TapeRecsRead)
		    restart = false;

	    }  /*** while restart ***/

	} /*** tape read error ***/
    } /* for */
	
         
    pBuffer->count = rp - pBuffer->content;

/***** Return the buffer to the writer...  *****/
    PassBufferToSaver (true);
}


/*********************************************
 * Special file creation with pre-allocation *
 *********************************************/
 
PublicFnDef int READER_SegmentCreate (
    char const *		fname,
    unsigned int		mode,
    int				bytes
)
{
    int				fd;

    if( -1 == (int)(fd = creat(fname, mode)) )
	return -1;

    if( bytes > 0 && ftruncate(fd,bytes) < 0)
	READER_FatalError ("File Preallocate");

    return fd;
}

PublicFnDef int READER_FileCreate (
    char const*			fname,
    unsigned int		mode,
    int				bytes
)
{
    int fd = creat(fname,mode);

    if (-1 == fd)
        return -1;
    if (bytes > 0 && lseek (fd, bytes, 0) < 0)
        READER_FatalError ("File Preallocate");
    lseek (fd, 0, 0);
    return fd;
}


/***********************************************
 *****  Reader Initializer and Terminator  *****
 ***********************************************/

#if defined(_WIN32)

PublicFnDef void READER_SpawnReader (
    int                         rsize,
    int                         tapes,
    int                         bufsize,
    int                         maxrecs,
    char const                  *filename,
    char const                  *tapename,
    int                         rewind
)
{
    RSIZE = rsize;
    TAPES = tapes;
    BUFSIZE = bufsize;
    MAXRECS = maxrecs;
    REWIND = rewind;
    FILENAME = strdup (filename);
    TAPENAME = strdup (tapename);

    OpenPromptFile ();
    OpenInput (tapename, filename, tapes, rewind);

    ReaderExitStatus = NormalExitValue;
}
#else

PublicFnDef void READER_SpawnReader (
    int				rsize,
    int				tapes,
    int				bufsize,
    int				maxrecs,
    char const			*filename,
    char const			*tapename,
    int				rewind
)
{
    ReaderPid = fork ();

    if (ChildProcess)
    {
	DATABUFFER* pBuffer;

	OpenPromptFile ();
	OpenInput (tapename, filename, tapes, rewind);

	while (
	    ReadByteCnt > 0
	    && NRecs < maxrecs
	    && IsntNil (pBuffer = GetBufferToLoad ())
	) FillBuffer (
	    pBuffer, rsize, tapes, bufsize, maxrecs, filename, tapename, rewind
	);
            
	Prompt("\nRecords read = %d\n", NRecs);
	ClosePromptFile ();
	ReaderExit (NormalExitValue);
    }
    else if (ReaderPid < 0) READER_FatalError ("Reader Spawn");
}
#endif

PublicFnDef void READER_SpawnReader (
    int                         rsize,
    int                         tapes,
    int                         bufsize,
    int                         maxrecs,
    char const                  *filename,
    char const                  *tapename,
    int                         rewind,
    char			*skipPattern
)
{
    if ( IsntNil (SkipPattern = (char *) malloc (strlen (skipPattern) + 1) ))
    {
	strcpy (SkipPattern, skipPattern);
	READER_SpawnReader (
	   rsize, tapes, bufsize, maxrecs, filename, tapename, rewind
	);
    }
    else READER_FatalError ("Can't Allocate Skip Pattern");
}


PublicFnDef void READER_CleanupAndExit (
    void
)
{
    int	oldstatus;

    /***** KillReader() will set ReaderExitStatus to an error but if the  *****/
    /***** reader has already died then the following KillReader()    *****/
    /***** will do nothing, so we preserve the real ReaderExitStatus here. *****/
    oldstatus = ReaderExitStatus;
    
    KillReader();

    CleanupSemaphores ();
    
    DeleteBuffers ();

    exit(oldstatus);
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  reader.c 1 replace /users/ees/dbutil
  861020 14:00:59 ees program to spawn tape reader process

 ************************************************************************/
/************************************************************************
  reader.c 2 replace /users/jck/updates
  861103 09:04:18 jck Modified signal handling organization

 ************************************************************************/
/************************************************************************
  reader.c 3 replace /users/ees/dbutil
  861113 10:09:50 ees removed call to memlck

 ************************************************************************/
/************************************************************************
  reader.c 4 replace /users/jck/updates
  871006 10:10:47 jck Changed reporting of 'P Error'

 ************************************************************************/
/************************************************************************
  reader.c 5 replace /users/m2/dbup
  880426 12:16:50 m2 Apollo and VAX port

 ************************************************************************/
/************************************************************************
  reader.c 6 replace /users/jck/updates
  880510 08:59:40 jck Added Error Messages for the Apollo signals

 ************************************************************************/
/************************************************************************
  reader.c 7 replace /users/jck/dbupdate
  891009 12:56:17 jck Standardizer updates

 ************************************************************************/
