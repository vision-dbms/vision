/*****  Machine Specific Lock Management Services  *****/

#include "stdoss.h"
#include "MSsupport.h"
#include "SIlock.h"
#include "SIfile.h"

#ifdef VMS				/*****  VAX/VMS  *****/
#include lckdef
#include ssdef
#endif

/*********************
 *  Lock Management  *
 *********************/
#ifdef VMS				/*****  VAX/VMS  *****/

/*****  RMS Name Block Access Macros  *****/
#define FID0(nam)		((nam).nam$w_fid[0])
#define FID1(nam)		((nam).nam$w_fid[1])
#define FID2(nam)		((nam).nam$w_fid[2])
#define DVI_L(nam)		((nam).nam$t_dvi[0])
#define DVI(nam)		((nam).nam$t_dvi + 1)

/*****  Mnemonics  *****/
#define UNLOCKED		0

/***** Utility to build a unique global name based on a file's device
 ***** residence and its identification number.
 *
 *  Arguments:
 *	buffer			- pointer to the character array where the
 *				  constructed global name should be deposited.
 *	nam			- an RMS name block structure which contains a
 *				  file's device name and unique id.
 *
 *  Returns:
 *	Nothing.
 *
 *****/
PrivateFnDef void
    GenerateGlobalName (buffer, nam)
char *buffer;
struct NAM nam;
{
    char format[32];

    sprintf (format, "%%%d.%ds%%d%%d%%d", DVI_L(nam), DVI_L (nam));
    sprintf (buffer, 
	     format, 
	     DVI (nam), 
	     FID0 (nam), 
	     FID1 (nam), 
	     FID2 (nam));
}

/***** Macro to mark all sublocks of a lockSet as unlocked.
 *
 *  Arguments:
 *	lockSet		-  A pointer to a structure of type
 *			   STDOSS_LockStatusBlock.
 *
 *  Syntactic Context:
 *	Compound Statement.
 *
 *****/
#define InitializeSubLocks(lockSet)\
{\
    int i;\
\
    for (i = 0; i < STDOSS_LSB_NumSubLocks (lockSet); i++)\
    {\
	STDOSS_LSB_SubLock (lockSet, i) = UNLOCKED;\
    }\
}

/*****  Routine to acquire a lock set based on a particular file.
 *
 *  Arguments:
 *	lockSet		-  A pointer to an STDOSS_LockStatusBlock which has
 *			   been initialized with a file to which to attach a
 *			   set of locks.
 *
 *  Returns:
 *	If successful, 0.
 *	If not, -1.
 *
 *****/
PublicFnDef int
    STDOSS_AcquireLockSet (pathName,lockSet)
char *pathName;
STDOSS_LSB lockSet;
{
    char lockname [50], vmsFileName [256];
    $DESCRIPTOR (nameDesc, lockname);
    STATUS status;
    PrivateVarDef int lockSetAcquired = FALSE;
/****  Initialize RMS structures  ****/
    struct FAB fab = cc$rms_fab;
    struct NAM nam = cc$rms_nam;

    if (lockSetAcquired)
        return 0;
	
/****
 *  Open the file via RMS in order to get a NAM block. The NAM block contains
 *  a field for the device name and a field for the file id. These two pieces
 *  of information are used to construct the name needed to identify the lock
 *  across processes.
 ****/

/****  Get the path name  ****/
    getname (STDOSS_LSB_FileD (lockSet), vmsFileName, 1);
/****  Attach the NAM block we want filled in  ****/
    fab.fab$l_nam = &nam;
/****  Fill in the FAB's name fields  ****/
    fab.fab$l_fna = vmsFileName;
    fab.fab$b_fns = strlen (vmsFileName);
/****  Set file access field to read only  ****/
    fab.fab$b_fac = FAB$V_GET;
/****  
 *  Set shared field to shared read and write. 
 *  (This reduces the chances that the open will fail)
 ****/
    fab.fab$b_shr = FAB$V_SHRGET | FAB$V_SHRPUT;
/****  open the file.  The NAM block gets filled in  ****/
    HandleVMSRoutine (sys$open (&fab), "sys$open", ;);
/****  use the NAM block to generate a name for the lock  ****/
    GenerateGlobalName (lockname, nam);
    $LENGTH (nameDesc) = strlen (lockname);

/****  Acquire the lock  ****/
    HandleVMSRoutine 
        (sys$enq (0, LCK$K_CWMODE, lockSet, LCK$M_NOQUEUE,
    		  &nameDesc, 0, 0, 0, 0, 0, 0),
         "sys$enq",
         sys$close (&fab);
        );

/****  Allocate space for a set of sublocks  ****/
    STDOSS_LSB_SubLocks (lockSet) = 
		(unsigned int *) malloc (100 * sizeof(int));
    STDOSS_LSB_NumSubLocks (lockSet) = 99;

/****  Initialize them...  ****/
    InitializeSubLocks (lockSet);

/****  And clean up  ****/
    HandleVMSRoutine (sys$close (&fab), "sys$close", ;);
    lockSetAcquired = TRUE;
    return 0;
}

/*****  Routine to discard a lock set (No-op for the VAX)
 *
 *  Arguments:
 *	lockSet		-  A pointer to an STDOSS_LockStatusBlock which has
 *			   been initialized with a file to which to attach a
 *			   set of locks.
 *
 *  Returns:
 *	If successful, 0.
 *	If not, -1.
 *
 *****/
PublicFnDef int
    STDOSS_DiscardLockSet (lockSet)
STDOSS_LSB lockSet;
{
    return 0;
}

/****************************
 *  Low Level Lock Manager  *
 ****************************/

/****  Macro to record the lock id of a sublock in the lock set
 *
 *  Arguments:
 *	lockSet		- An STDOSS_LSB for the lock Set.
 *	lockNumber	- The index of the sublock to be recorded.
 *	sublock		- An STDOSS_LSB for the sublock.
 *
 *  Syntactic Context:
 *	Compound Statement.
 *
 ****/
#define	RememberSubLockId(lockSet, lockNumber, sublock)\
	if (STDOSS_LSB_NumSubLocks (lockSet) <= lockNumber)\
	{\
	    int i;\
	/****  Grow the sublock array if necessary  ****/\
	    STDOSS_LSB_SubLocks (lockSet) = \
		(unsigned int *) realloc (STDOSS_LSB_SubLocks (lockSet),\
		(lockNumber + 2) * sizeof(int));\
	/****  Mark the new sublocks as unlocked  ****/\
	    for (i = 1 + STDOSS_LSB_NumSubLocks (lockSet); i <= lockNumber; i++)\
		STDOSS_LSB_SubLock (lockSet, lockNumber) = UNLOCKED;\
	    STDOSS_LSB_NumSubLocks (lockSet) = lockNumber + 1;\
	}\
	/****  Record the sublock's ID  ****/\
	STDOSS_LSB_SubLock (lockSet, lockNumber) = \
		STDOSS_LSB_LockId (sublock)

/*---------------------------------------------------------------------------
 *****  Routine to perform a lock operation.
 *
 *  Arguments:
 *	lockSet			- the set of locks from which a lock will be
 *				  selected.
 *	lockNumber		- the lock to select.  A negative 'lockNumber'
 *				  selects all possible locks.
 *	lockOp			- the lock operation to be performed (F_LOCK,
 *				  F_TLOCK, F_TEST, F_ULOCK).
 *
 *  Returns:
 *	TRUE if the operation completed successfully, FALSE if an 'F_TLOCK' was
 *	requested which did not procur the requested lock, -1 if an error occured.
 *
 *****/
PublicFnDef int
    STDOSS_LockOp (lockSet, lockNumber, lockOp)
STDOSS_LSB lockSet;
int lockNumber, lockOp;
{
    char lockName [16];
    $DESCRIPTOR (nameDesc, lockName);
    STDOSS_LockStatusBlock lockStatusBlock;
    STATUS status;

/****  The only valid operation on all locks is to unlock them  ****/
    if (-1 == lockNumber && lockOp != F_ULOCK)
    {
	errno = EINVAL;
	return -1;
    }
/****  If a single lock is chosen, generate a name for it  ****/
    if (-1 != lockNumber)
    {
	sprintf (lockName, "%d", lockNumber);
	$LENGTH (nameDesc) = strlen (lockName);
    }
    switch (lockOp)
    {
    case F_LOCK:
    /**** If the requested sublock is already in the lockset, do nothing ****/
	if (STDOSS_LSB_NumSubLocks (lockSet) > lockNumber &&
	    STDOSS_LSB_SubLock (lockSet, lockNumber))
	    break;
    /**** Otherwise wait for the lock ... ****/
	HandleVMSRoutine
	     (sys$enqw (0, LCK$K_EXMODE, &lockStatusBlock, 0, &nameDesc, 
			STDOSS_LSB_LockId (lockSet), 0, 0, 0, 0, 0),
	      "sys$enqw",
	      ; );
	HandleVMSRoutine
	     (STDOSS_LSB_VMScond (&lockStatusBlock),
	      "sys$enqw",
	      ; );
    /**** And record it  ****/
	RememberSubLockId (lockSet, lockNumber, &lockStatusBlock);
	break;
    case F_TLOCK:
    /****  Attempt to get a lock without waiting  ****/
	status = sys$enq (0, LCK$K_EXMODE, &lockStatusBlock, LCK$M_NOQUEUE, 
			  &nameDesc, STDOSS_LSB_LockId (lockSet), 
			  0, 0, 0, 0, 0);
    /****  
     *  If the status is SS$_NOTQUEUED, the lock wasn't immediately available.
     ****/
	if (status == SS$_NOTQUEUED) return FALSE;
	HandleVMSRoutine (status, "sys$enq", ; );
    /**** The lock was granted, so record it.  ****/
	RememberSubLockId (lockSet, lockNumber, &lockStatusBlock);
	break;
    case F_ULOCK:
        if (lockNumber == -1)
	{
	/****  Cancel All locks  *****/
	    HandleVMSRoutine
		 (sys$deq (STDOSS_LSB_LockId (lockSet), 0, 0, LCK$M_DEQALL),
        		   "sys$deq",
        		   ; );
	/****  Mark All locks as released  ****/
	    InitializeSubLocks (lockSet);
	}
	else if (STDOSS_LSB_SubLock (lockSet, lockNumber) != UNLOCKED)
	{
	/**** Unlock the sublock  ****/
	    HandleVMSRoutine
		 (sys$deq (STDOSS_LSB_SubLock (lockSet, lockNumber), 0, 0, 0),
        		   "sys$deq",
        		   ; );
	/****  Mark the sublock as released  ****/
	    STDOSS_LSB_SubLock (lockSet, lockNumber) = 0;
	}
	break;
    case F_TEST:  /**** Not implemented  ****/
    default:
    /****  Unknown Operation  *****/
	errno = EINVAL;
	return -1;
	break;
    }
    return TRUE;
}
#endif

#ifdef hp9000s500
/*****  A no-op routine on the HP. *****/
PublicFnDef int
    STDOSS_AcquireLockSet (pathName, lockSet)
char *pathName;
STDOSS_LSB lockSet;
{
    return 0;
}

/*****  Routine to discard a lock set (A no-op on the HP)
 *
 *  Arguments:
 *	lockSet		-  A pointer to an STDOSS_LockStatusBlock which has
 *			   been initialized with a file to which to attach a
 *			   set of locks.
 *
 *  Returns:
 *	If successful, 0.
 *	If not, -1.
 *
 *****/
PublicFnDef int
    STDOSS_DiscardLockSet (lockSet)
STDOSS_LSB lockSet;
{
    return 0;
}

/*---------------------------------------------------------------------------
 *****  Routine to perform a lock operation.
 *
 *  Arguments:
 *	lockSet			- the set of locks from which a lock will be
 *				  selected.
 *	lockNumber		- the lock to select.  A negative 'lockNumber'
 *				  selects all possible locks.
 *	lockOp			- the lock operation to be performed (F_LOCK,
 *				  F_TLOCK, F_TEST, F_ULOCK).
 *
 *  Returns:
 *	TRUE if the operation completed successfully, FALSE if an 'F_TLOCK' was
 *	requested which did not procur the requested lock.
 *
 *****/
PublicFnDef int
    STDOSS_LockOp (lockSet, lockNumber, lockOp)
STDOSS_LSB lockSet;
int lockNumber, lockOp;
{
    int failure;

    if (-1 == lseek (*lockSet, lockNumber < 0 ? 0 : lockNumber, SEEK_SET))
	 return -1;

    failure = lockf (*lockSet, lockOp, lockNumber < 0 ? 0 : 1);

    if (failure && (lockOp != F_TLOCK || errno != EACCES))
    {
	return -1;
    }

    return !failure;
}
#endif

#ifdef apollo				/*****  DOMAIN/IX  *****/

#include "fclient.h"

PublicFnDef int
    STDOSS_AcquireLockSet (pathName, lockSet)
char *pathName;
STDOSS_LSB lockSet;
{
    return FCLIENT_AccessLockSet (pathName);
}

/*****  Routine to discard a lock set
 *
 *  Arguments:
 *	lockSet		-  A pointer to an STDOSS_LockStatusBlock which has
 *			   been initialized with a file to which to attach a
 *			   set of locks.
 *
 *  Returns:
 *	If successful, 0.
 *	If not, -1.
 *
 *****/
PublicFnDef int
    STDOSS_DiscardLockSet (lockSet)
STDOSS_LSB lockSet;
{
    return FCLIENT_DiscardLockSet ();
}

PublicFnDef int
    STDOSS_LockOp (lockSet, lockNumber, lockOp)
STDOSS_LSB lockSet;
int lockNumber, lockOp;
{
    int failure;
    failure = FCLIENT_lock (*lockSet, lockNumber, lockOp);

    if (failure && (lockOp != F_TLOCK || errno != EACCES))
    {
	return -1;
    }

    return !failure;
}

#endif

#ifdef sun
/*****  A no-op routine on the Sun. *****/
PublicFnDef int
    STDOSS_AcquireLockSet (pathName, lockSet)
char *pathName;
STDOSS_LSB lockSet;
{
    return 0;
}

/*****  Routine to discard a lock set (A no-op on the HP)
 *
 *  Arguments:
 *	lockSet		-  A pointer to an STDOSS_LockStatusBlock which has
 *			   been initialized with a file to which to attach a
 *			   set of locks.
 *
 *  Returns:
 *	If successful, 0.
 *	If not, -1.
 *
 *****/
PublicFnDef int
    STDOSS_DiscardLockSet (lockSet)
STDOSS_LSB lockSet;
{
    return 0;
}

/*---------------------------------------------------------------------------
 *****  Routine to perform a lock operation.
 *
 *  Arguments:
 *	lockSet			- the set of locks from which a lock will be
 *				  selected.
 *	lockNumber		- the lock to select.  A negative 'lockNumber'
 *				  selects all possible locks.
 *	lockOp			- the lock operation to be performed (F_LOCK,
 *				  F_TLOCK, F_TEST, F_ULOCK).
 *
 *  Returns:
 *	TRUE if the operation completed successfully, FALSE if an 'F_TLOCK' was
 *	requested which did not procur the requested lock.
 *
 *****/
PublicFnDef int
    STDOSS_LockOp (lockSet, lockNumber, lockOp)
STDOSS_LSB lockSet;
int lockNumber, lockOp;
{
    int failure;

    if (-1 == lseek (*lockSet, lockNumber < 0 ? 0 : lockNumber, SEEK_SET))
	 return -1;

    failure = lockf (*lockSet, lockOp, lockNumber < 0 ? 0 : 1);

    if (failure && (lockOp != F_TLOCK || errno != EACCES))
    {
	return -1;
    }

    return !failure;
}
#endif /* sun */

/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  MSlock.c 1 replace /users/jck/system
  880330 21:52:16 jck SIutil.h

 ************************************************************************/
/************************************************************************
  MSlock.c 2 replace /users/jck/system
  880407 10:45:08 jck Cleaned Up VAX interface

 ************************************************************************/
/************************************************************************
  MSlock.c 3 replace /users/jck/system
  880415 10:58:19 jck Fixed a bug on the VAX which occured during an aborted save

 ************************************************************************/
/************************************************************************
  MSlock.c 4 replace /users/jck/system
  880419 07:44:38 jck Provided temporary lock routines for the apollo

 ************************************************************************/
/************************************************************************
  MSlock.c 5 replace /users/jck/system
  880426 09:05:14 jck Fixed typo in VAX implementation

 ************************************************************************/
/************************************************************************
  MSlock.c 6 replace /users/jck/system
  880526 14:43:43 jck Interfaced with the new Apollo lock server

 ************************************************************************/
/************************************************************************
  MSlock.c 7 replace /users/jck/system
  880915 12:04:25 jck Modify lock-set acquisition protocol

 ************************************************************************/
/************************************************************************
  MSlock.c 8 replace /users/m2/backend
  890503 14:22:20 m2 Minor VAX fixes

 ************************************************************************/
/************************************************************************
  MSlock.c 9 replace /users/m2/backend
  890927 14:29:02 m2 SUN port (preliminary)

 ************************************************************************/
