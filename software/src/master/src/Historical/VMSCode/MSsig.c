/*****  Machine Specific Signal Processing  *****/

#include "stdoss.h"

#include <signal.h>

/*********************
 ***  Signal Handling  *
 *********************/
#ifdef VMS

PublicFnDef void
    sigvector (sig, vec, ovec)
int sig;
struct sigvec *vec, *ovec;
{
    if (IsntNil (ovec))
    {
        ovec->sv_handler = SIG_IGN;
	ovec->sv_mask	 = 0;
	ovec->sv_onstack = 0;
    }
}

/*PublicFnDef int
    sigsetmask ()
{
    return 0;
}*/

#if defined(FRONTEND) || defined(BACKEND)

#include <stdio.h>
#include <ssdef.h>
#include <lckdef.h>
#include <lkidef.h>
#include "SIsig.h"
#include "MSsupport.h"

/*******************************************************
 *  VAX signals between processes simulated with locks *
 *******************************************************/

/********************************************************************** 
 *** NOTE: These routines are designed for use between the frontend and
 *** the backend only.  Therefore, when there are sections like
 ***	#ifdef FRONTEND
 ***	...
 ***	#endif
 ***	#ifdef BACKEND
 ***	...
 ***	#endif
 ***
 *** Also, much of the same code and variables are used in both but since
 *** the programs are compiled separately, they do not share anything.
 ***
 *** The frontend will call the signal handler routine as a procedure instead
 *** of raising a signal because signals are risky while inside an AST.
 *** (At least that was my impression from the manual).
 **********************************************************************/


/*** Each "signal" (i.e. lock) has its own lock status block which contains
 *** the unique lock id and status condition
 ***/
struct _lksb {
	unsigned short	cond;
	unsigned short	res;
	unsigned int	lockid;
};
PrivateVarDef struct _lksb	SIGCLDlksb, SIGINTlksb;

/*** Necessary structure in system call arguments but ignored by us
 ***/
struct _retlen {
	unsigned int	len : 16,
			blen : 15,
			buftoosmall : 1;
};
PrivateVarDef struct _retlen	retlen;

/*** A list of items to get from the sys$getlkiw() routine.
 ***/
struct _itemlist {
	unsigned short	buflen;
	unsigned short	itemcode;
	char		*bufaddr;
	struct _retlen	*retlenaddr;
};
PrivateVarDef struct _itemlist	itemlist[2];

/*** Another necessary but ignored structure
 ***/
struct _iosb {
	unsigned int	cond;
	unsigned short	res1, res2;
};
PrivateVarDef struct _iosb	iosb;

/*** Each sys$eqn() and sys$getlkiw() gets its own local event flag.
 ***/
#define	SIGCLDefn	13
#define	SIGINTefn	14
#define	lkiefn		15

/*** The name of the lock resource is placed in tese descriptors.
 *** I have chosen the format of signal name followed by child process id
 *** example: SIGINT206005f2
 *** The parent gets the childs pid from the vfork() call and the child simply
 *** calls getpid() for its own pid.
 ***/
PrivateVarDef char	SIGCLDresnam[32], SIGINTresnam[32];
PrivateVarDef $DESCRIPTOR(SIGCLDresdesc,SIGCLDresnam);
PrivateVarDef $DESCRIPTOR(SIGINTresdesc,SIGINTresnam);

/*** In order to synchonize the locks between the two processes, one process
 *** will obtain a lock and then wait until the lockcount for that lock
 *** becomes 2.  It then knows that the other process has also obtained that
 *** lock
 ***/
PrivateVarDef unsigned int	 IsVolatile(lockcount);

/*** The synchronization is done by using two lock modes. The null mode
 *** simply lets you obtain the lock but not do much with it (except obtain
 *** information).  Exclusive mode prevents another process from obtaining
 *** any other mode on the lock except null mode.
 ***/
PrivateVarDef int	nullmode = LCK$K_NLMODE;
PrivateVarDef int	exclmode = LCK$K_EXMODE;

/*** If any of the locking functions fail, then set the flag and do not use
 *** that lock at all.  Therefore, it is possible that the frontend may hang
 *** because the SIGCLD lock may not be set and/or the backend may never 
 *** receive an interrupt if the SIGINT lock is not setup.
 *** This funtionality may be changed in the future (i.e. have the process
 *** die if the appropriate locks cannot be set).
 ***/
PrivateVarDef int	IsVolatile(SIGCLDfail) = 0;
PrivateVarDef int	IsVolatile(SIGINTfail) = 0;

/**********************************************************************
 *** Obtain a lock for the signal specified in astparm.
 ***/
PrivateFnDef int
getlock(wait,mode,flags,astproc,astparm)
int	wait,	/*** If TRUE then wait until lock is granted. ***/
	mode,	/*** Either nullmode or exclmode ***/
	flags;	/*** Either 0 for obtaining a new lock or 
		 *** (LCK$M_CONVERT|LCK$M_NODLCKWT) which converts the mode
		 *** of an already obtained lock and informs the lock manager
		 *** to ignore deadlock detection.
		 ***/
int	(*astproc)();	/*** AST procedure to executewhen lock is granted.
			 *** Should only be specified if wait == FALSE.
			 ***/
int	astparm;	/*** Parameter passed to astproc().  This should always
			 *** be specified (even if astproc == NULL) and should
			 *** always be the signal number.
			 ***/
{
	int			efn, status;
	struct dsc$descriptor_s	*resdesc;
	struct _lksb		*lksb;

	/*** Determine which global variables to use ***/
	switch(astparm)
	{
		case SIGCLD:
			resdesc = &SIGCLDresdesc;
			lksb = &SIGCLDlksb;
			efn = SIGCLDefn;
			break;
		case SIGINT:
			resdesc = &SIGINTresdesc;
			lksb = &SIGINTlksb;
			efn = SIGINTefn;
			break;
	}
	
	/*** If we wait for the lock then use sys$enqw() ***/
	if( wait )
		status = sys$enqw(	efn,
				   	mode,
			   		lksb,
			   		flags,
					resdesc,
			   		0,
			   		astproc,
			   		astparm,
			   		0,
			   		0,
			   		0 );
	else /*** Otherwise use sys$enq() ***/
		status = sys$enq(	efn,
				   	mode,
			   		lksb,
			   		flags,
			   		resdesc,
			   		0,
			   		astproc,
			   		astparm,
			   		0,
			   		0,
			   		0 );

	if( (status != SS$_NORMAL) || 
	    ((lksb->cond != SS$_NORMAL) && (lksb->cond > 0)) )
	{
		errno = EVMSERR;
		if( status != SS$_NORMAL )
		{
			vaxc$errno = status;
			perror("enq");
		}
		if( lksb->cond != SS$_NORMAL )
		{
			vaxc$errno = lksb->cond;
			perror("enq:lksb");
		}
		return(-1);
	}
	return(0);
}

/**********************************************************************
 *** Throw away a currently owned lock.
 ***/
PrivateFnDef void
remlock(lockid)
int	lockid;	/*** Lockid of lock to discard ***/
{
	int	status;
	status = sys$deq(	lockid,
			   	0,
			   	0,
			   	0 );
	if( status != SS$_NORMAL )
	{
		errno = EVMSERR;
		vaxc$errno = status;
		perror("deq");
	}
}

PrivateFnDef int
getlockcount(lockid)
int	*lockid;	/*** Lockid from which to obtain information.
			 *** NOTE: getlkiw() must be able to write to lockid
			 *** therefore it is a pointer.
			 ***/
{
	int	status;

	lockcount = 0;

	/*** Obtain the lock count on this lock ***/
	itemlist[0].buflen = 4;
	itemlist[0].itemcode = LKI$_LCKCOUNT;
	itemlist[0].bufaddr = (char *)&lockcount;
	itemlist[0].retlenaddr = &retlen;
	/*** A NULL item signifies the end of the list. ***/
	itemlist[1].buflen = 0;
	itemlist[1].itemcode = 0;
	itemlist[1].bufaddr = 0;
	itemlist[1].retlenaddr = 0;

	status = sys$getlkiw(	lkiefn,
				lockid,
				itemlist,
			   	&iosb,
			   	0,
			   	0,
			   	0 );
	if( status != SS$_NORMAL )
	{
		errno = EVMSERR;
		vaxc$errno = status;
		perror("getlki");
		return(-1);
	}
	return(0);
}

#ifdef FRONTEND

/*** NOTE: I can't include the local rsInterface.h file here because the
 *** f@$#ing GenMakeFragment program doesn't know about conditional
 *** compilation!  Therefore, the following two declarations are necessary.
 ***/
PublicVarDecl int	RSpid;
PublicFnDecl void	RS_callSignalHandler();

PrivateVarDef int	IsVolatile(InSIGINT) = FALSE;

runPseudoSignalHandler(sig)
int	sig;
{
	switch(sig)
	{
		case SIGCLD:
			/*** NOTE: When lock is granted the lock status block
			 *** gets filled in with the condition code
			 ***/
			if( (SIGCLDlksb.cond != SS$_NORMAL) &&
			    (SIGCLDlksb.cond > 0) )
			{
				errno = EVMSERR;
				vaxc$errno = SIGCLDlksb.cond;
				perror("rPSH:SIGCLDlksb");
				return(-1);
			}
			/*** Call handler instead of generating an interrupt ***/
			RS_callSignalHandler(SIGCLD);
			break;
		case SIGINT:
			/*** Finish processing one interrupt before another ***/
			if( InSIGINT )
				return(0);

			InSIGINT = TRUE;

			/*** NOTE: When lock is granted the lock status block
			 *** gets filled in with the condition code
			 ***/
			if( (SIGINTlksb.cond != SS$_NORMAL) &&
			    (SIGINTlksb.cond > 0) )
			{
				errno = EVMSERR;
				vaxc$errno = SIGINTlksb.cond;
				perror("rPSH:SIGINTlksb");
			}

			/*** Discard lock which sets of the backend's AST ***/
			if( SIGINTfail != -1 )
				remlock(SIGINTlksb.lockid);

			/*** And then obtain lock again.
			 *** NOTE: The may be a timing problem here but I doubt
			 *** it.  If the lock is dequeued above and the lock
			 *** manager somehow grants the following request before
			 *** the queued backend request is granted then the 
			 *** backend won't get the interrupt (which is no big
			 *** deal since you can always try again).  It seems
			 *** that the lock manager does this correctly, however,
			 *** so no synchronization should be needed.
			 ***/
			SIGINTfail = getlock(	TRUE,
						exclmode,
						0,
						NULL,
						SIGINT );

			InSIGINT = FALSE;
			break;
	}
}

/**********************************************************************
 *** The frontend first obtains a nullmode lock, then waits for the backend
 *** to obtain an exclmode lock (lockcount == 2), then if queues a request
 *** for an exclmode lock.  When that request is granted (when the backend dies
 *** and its exclmode lock is released) the AST routine is called which handles
 *** the death-of-child processing.
 ***/
PrivateFnDef int
setupSIGCLD(restart)
int	restart;	/*** Did backend die? ***/
{
	int	i = 10000;

	/*** If the backend died then delete old lock ***/
	if( restart && (SIGCLDfail != -1) )
		remlock(SIGCLDlksb.lockid);

	/*** We are starting fresh, so reset fail flag ***/
	SIGCLDfail = FALSE;

	/*** Create lock resource name using child's pid ***/
	sprintf(SIGCLDresnam,"SIGCLD%08x",RSpid);

	/*** Obtain a nullmode lock so we can get the lockid needed for 
	 *** sys$getlkiw()
	 ***/
	SIGCLDfail = getlock(	TRUE,
				nullmode,
				0,
				NULL,
				SIGCLD );
	if( SIGCLDfail == -1 )
		return(SIGCLDfail);

	/*** It would be preferable to use an alarm() here but I don't
	 *** trust VAX signals for ANYTHING.  Therefore, we loop a huge
	 *** number of times.
	 ***/
	while( --i > 0 )
	{
		if( (SIGCLDfail = getlockcount(&SIGCLDlksb.lockid)) == -1 )
			return(SIGCLDfail);
		if( lockcount >= 2 )
			break;
	}
	if( i <= 0 )
		return( (SIGCLDfail = -1) );
	
	SIGCLDfail = getlock(	FALSE,
				exclmode,
				(LCK$M_CONVERT|LCK$M_NODLCKWT),
				runPseudoSignalHandler,
				SIGCLD );
	return(SIGCLDfail);
}

/**********************************************************************
 *** Obtain an exclmode lock.  When an interrupt occurs in the frontend, the
 *** frontend will release the lock which sets off the AST in the backend
 *** Then the lock is reobtained.
 ***/
setupSIGINT(restart)
int	restart;	/*** Did backend die? ***/
{
	/*** If the backend died then delete old lock ***/
	if(restart && (SIGINTfail != -1) )
		remlock(SIGINTlksb.lockid);

	/*** We are starting fresh, so reset fail flag ***/
	SIGINTfail = FALSE;

	/*** Create lock resource name using child's pid ***/
	sprintf(SIGINTresnam,"SIGINT%08x",RSpid);

	SIGINTfail = getlock(	TRUE,
				exclmode,
				0,
				NULL,
				SIGINT );
	return(SIGINTfail);
}

/**********************************************************************
 *** Called by the frontend to setup the locks after the backend is forked
 ***/
PublicFnDef int
STD_SetupVAXSignals(restart)
int	restart;	/*** Did backend die? ***/
{
	setupSIGCLD(restart);
	setupSIGINT(restart);
	return( (SIGCLDfail|SIGINTfail) );
}

#endif /* FRONTEND */

#ifdef BACKEND

/**********************************************************************
 *** Obtain an exclmode lock.  If the backend dies then this lock gets released
 *** automatically and the frontend's AST will execute.
 ***/
PrivateFnDef int
setupSIGCLD()
{
	/*** Create lock resource name using child's pid ***/
	sprintf(SIGCLDresnam,"SIGCLD%08x",getpid());

	SIGCLDfail = getlock(	TRUE,
				exclmode,
				0,
				NULL,
				SIGCLD );

	return(SIGCLDfail);
}

/**********************************************************************
 *** When frontend releases the lock, the backend "generates" an interrupt
 *** (with the procedure call), deletes the lock, and then sets itself up again.
 ***/
PrivateFnDef int
runSIGINT(parm)
int	parm;
{
	/*** Generate interrupt signal
	 *** NOTE: I originally simulated an interrupt by calling the signal
	 *** handler procedure.  However, if many interrupt signals were sent
	 *** from the frontend before the backend cleaned up, then several
	 *** interrupt messages were printed and, more drastically, the alarm()
	 *** in the interrupt handler didn't seem to get reset causing the
	 *** backend to die horribly.  (I'm not sure about the alarm being
	 *** mishandled, but it takes about 60 seconds before the backend dies
	 *** which is the alarm() time the backend sets int the handler.)
	 *** Generating a real signal seems to work correctly no matter how many
	 *** interrupts the frontend generates, however, I am still uneasy about
	 *** generating interrupts inside of an AST.
	 ***/
	gsignal(SIGINT);

	/*** Delete lock so that frontend can reobtain it. ***/
	remlock(SIGINTlksb.lockid);
	
	/*** Setup lock again ***/
	return( setupSIGINT() );
}

/**********************************************************************
 *** Get a nullmode lock, then wait for frontend to get an exclmode lock.
 *** Then queue a request for an exclmode lock.  When frontend releases lock,
 *** the AST routine gets executed.
 ***/
PrivateFnDef int
setupSIGINT()
{
	int	i = 10000;

	/*** Create lock resource name using child's pid ***/
	sprintf(SIGINTresnam,"SIGINT%08x",getpid());

	/*** Obtain a nullmode lock so we can get the lockid needed for 
	 *** sys$getlkiw()
	 ***/
	SIGINTfail = getlock(	TRUE,
				nullmode,
				0,
				NULL,
				SIGINT );

	/*** It would be preferable to use an alarm() here but I don't
	 *** trust VAX signals for ANYTHING.  Therefore, we loop a huge
	 *** number of times.
	 ***/
	while( --i > 0 )
	{
		if( (SIGINTfail = getlockcount(&SIGINTlksb.lockid)) == -1 )
			return(SIGINTfail);
		if( lockcount >= 2 )
			break;
	}
	if( i <= 0 )
		return( (SIGINTfail = -1) );

	SIGINTfail = getlock(	FALSE,
				exclmode,
				(LCK$M_CONVERT|LCK$M_NODLCKWT),
				runSIGINT,
				SIGINT );
	return(SIGINTfail);
}

/**********************************************************************
 *** Called by the backend only if it is talking to the frontend
 ***/
PublicFnDef int
STD_SetupVAXSignals(dummy)
int	dummy;	/*** Dummy parameter ***/
{
	setupSIGCLD();
	setupSIGINT();
	return( (SIGCLDfail|SIGINTfail) );
}
#endif /* BACKEND */

#endif /* FRONTEND || BACKEND */

#endif /* VMS */

#if defined(hp9000s500) || defined(apollo) || defined(sun)

#if defined(FRONTEND) || defined(BACKEND)

/**********************************************************************
 *** No meaning for anything but the VAX
 ***/
PublicFnDef int
STD_SetupVAXSignals(dummy)
int	dummy;	/*** Dummy parameter ***/
{
	return(0);
}

#endif /* FRONTEND || BACKEND */

#endif /* hp || apollo || sun */

/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  MSsig.c 1 replace /users/jck/system
  880330 21:54:04 jck SIutil.h

 ************************************************************************/
/************************************************************************
  MSsig.c 2 replace /users/m2/dbup
  880421 17:08:03 m2 Apollo and VAX (and HP) port release

 ************************************************************************/
/************************************************************************
  MSsig.c 3 replace /users/m2/backend
  890503 14:27:49 m2 VAX signals between the FRONTEND and BACKEND

 ************************************************************************/
/************************************************************************
  MSsig.c 4 replace /users/m2/backend
  890828 16:57:15 m2 Removed extraneous '&'

 ************************************************************************/
/************************************************************************
  MSsig.c 5 replace /users/m2/backend
  890927 14:34:08 m2 SUN port (preliminary)

 ************************************************************************/
