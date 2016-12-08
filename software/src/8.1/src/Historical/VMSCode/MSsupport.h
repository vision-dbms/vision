/*****  Machine Specific Header File  *****/
#ifndef ms_H
#define ms_H

/****************
 *  Invariants  *
 ****************/

/********************
 *  Customizations  *
 ********************/

#ifdef hp9000s500		/*****  HP9000 Series 500  *****/
#endif

#ifdef VMS			/*****  VAX/VMS  *****/

typedef int STATUS;
#define Success(status)		((status) & STS$M_SUCCESS)
#define Failure(status)		! Success (status)
#define EndOfFile(status)	((status) == RMS$_EOF)
#define NoSuchSection(status)	((status) != SS$_NOSUCHSEC)

#define SpecialHandleVMSRoutine(routine, extraCond, msg, codeBody, returnCode)\
{\
    STATUS istatus = routine;\
    /*printf ("VMS call [%s] status: %d(%X)\n", msg, istatus, istatus);*/\
    if (Failure (istatus) && extraCond (istatus))\
    {\
	vaxc$errno = istatus;\
	errno = EVMSERR;\
	perror (msg);\
	codeBody;\
	return returnCode;\
    }\
}
#define TrueCond(x) TRUE

#define HandleVMSRoutine(routine, msg, codeBody)\
		SpecialHandleVMSRoutine (routine, TrueCond, msg, codeBody, -1)

 
/*****  macros set up and access fields of VAX scalar or string descriptor  **/
#include descrip
#define $LDESCRIPTOR(var, len, addr)\
	struct dsc$descriptor_s var =\
		{len, DSC$K_DTYPE_T, DSC$K_CLASS_S, addr}

#define $LENGTH(dsc) (dsc).dsc$w_length
#define $ADDRESS(dsc) (dsc).dsc$a_pointer


#endif

#ifdef apollo			/*****  DOMAIN/IX  *****/
#endif

#ifdef lint
#ifdef malloc
#undef malloc
#endif
#ifdef realloc
#undef realloc
#endif
#ifdef calloc
#undef calloc
#endif
#define	malloc(x)	0
#define	realloc(x,y)	0
#define	calloc(x,y)	0
#endif /* lint */

#endif /*****  ms_H  *****/


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  MSsupport.h 1 replace /users/jck/system
  880330 21:54:29 jck SIutil.h

 ************************************************************************/
/************************************************************************
  MSsupport.h 2 replace /users/m2/dbup
  880421 17:11:08 m2 Apollo and VAX (and HP) port release

 ************************************************************************/
/************************************************************************
  MSsupport.h 3 replace /users/m2/frontend/sys
  891011 14:11:17 m2 Lint fixes

 ************************************************************************/
