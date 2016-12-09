/*****  Machine Specific Utilities Header File  *****/
#ifndef siUTIL_H
#define siUTIL_H

/************************************
 *****  Timers and Time Stamps  *****
 ************************************/
/*---------------------------------------------------------------------------
 *  Required Definitions:
 *	struct timer		- a process timer.
 *				  Corresponding fields of two process timers
 *				  can be subtracted to yield the number of
 *				  milliseconds (subject to system clock
 *				  granularity) separating the two timer
 *				  readings.
 *	struct time		- a time stamp.
 *				  A timestamp is assumed to record the system's
 *				  current notion (GMT?) of the absolute time in
 *				  microseconds (subject to system clock
 *				  granularity).
 *	int gettimer (struct timer *timer)
 *				- routine to access the current timer value.
 *				  Returns 0 if successful; -1 and 'errno' if
 *				  not.
 *	int gettime (struct time *time)
 *				- routine to access the current absolute time.
 *				  Returns 0 if successful; -1 and 'errno' if
 *				  not.
 *---------------------------------------------------------------------------
 */

/****************
 *  Invariants  *
 ****************/

/*****  Timer  *****/
struct timer {
    unsigned long
	elapsed, user, system, cuser, csystem;
};

/*****  Time  *****/
struct time {
    unsigned long
	seconds, microseconds;
};

PublicFnDecl int
    gettimer (),
    gettime ();

/********************
 *  Customizations  *
 ********************/

#ifdef hp9000s500		/*****  HP9000 Series 500  *****/
#endif

#ifdef VMS			/*****  VAX/VMS  *****/
#define gmtime localtime
#endif

#ifdef apollo			/*****  DOMAIN/IX  *****/
#endif

/*****************************************
 *****  Character String Operations  *****
 *****************************************/
/*---------------------------------------------------------------------------
 *  Required Definitions:
 *	strcpy			- see STRING(3C).
 *	strncpy			- see STRING(3C).
 *	strcat			- see STRING(3C).
 *	strncat			- see STRING(3C).
 *	strchr			- see STRING(3C).
 *	strrchr			- see STRING(3C).
 *	strpbrk			- see STRING(3C).
 *	strcmp			- see STRING(3C).
 *	strncmp			- see STRING(3C).
 *	strlen			- see STRING(3C).
 *	strspn			- see STRING(3C).
 *	strcspn			- see STRING(3C).
 *---------------------------------------------------------------------------
 */

/****************
 *  Invariants  *
 ****************/

PublicFnDecl char
    *strcpy	(),
    *strncpy	(),
    *strcat	(),
    *strncat	(),
    *strchr	(),
    *strrchr	(),
    *strpbrk	();
PublicFnDecl int
    strcmp	(),
    strncmp	(),
    strlen	(),
    strspn	(),
    strcspn	();

/********************
 *  Customizations  *
 ********************/

#ifdef hp9000s500		/*****  HP9000 Series 500  *****/
#endif

#ifdef VMS			/*****  VAX/VMS  *****/
#endif

#ifdef apollo			/*****  DOMAIN/IX  *****/
#endif

/**********************************
 *****  Formatting Utilities  *****
 **********************************/
/*---------------------------------------------------------------------------
 *  Required Definitions:
 *	int vprintf (char *fmt, va_list ap)
 *				- see VPRINTF (3S)
 *	int vsprintf (char *buffer, char *fmt, va_list ap)
 *				- see VPRINTF (3S)
 *	int vfprintf (FILE *buffer, char *fmt, va_list ap)
 *				- see VPRINTF (3S)
 *---------------------------------------------------------------------------
 */

/****************
 *  Invariants  *
 ****************/

PublicFnDecl int
    vprintf (),
    vsprintf (),
    vfprintf ();

/*** For the backend, use m2's printf routines (which do not break)
 *** unless the module is compiled with the USEORIGINALPRINTF flag.
 ***/
#ifdef BACKEND

#ifdef USEORIGINALPRINTF

#define STD_printf	printf
#define STD_fprintf	fprintf
#define STD_sprintf	sprintf
#define STD_vprintf	vprintf
#define STD_vfprintf	vfprintf
#define STD_vsprintf	vsprintf

#else /* USEORIGINALPRINTF */

PublicFnDecl int STD_printf();
PublicFnDecl int STD_fprintf();
PublicFnDecl int STD_sprintf();
PublicFnDecl int STD_vprintf();
PublicFnDecl int STD_vfprintf();
PublicFnDecl int STD_vsprintf();

#endif /* USEORIGINALPRINTF */

#endif /* BACKEND */

/********************
 *  Customizations  *
 ********************/

/*** Never use VMS's vprintf functions even if USEORIGINALPRINTF defined ***/
#ifdef VMS

/****** VMS supplied versions are broken, so we'll use our own  *****/
PublicFnDecl int
    VMS_vprintf (),
    VMS_vsprintf (),
    VMS_vfprintf ();

#define vprintf VMS_vprintf
#define vsprintf VMS_vsprintf
#define vfprintf VMS_vfprintf

#endif /*VMS */

#ifdef BACKEND

#ifndef USEORIGINALPRINTF

#define siUTIL_PrepRealNumForPrinting(value) value

#else /* USEORIGINALPRINTF */

#ifdef hp9000s500		/*****  HP9000 Series 500  *****/

#define siUTIL_PrepRealNumForPrinting(value) value

#endif /* hp */

#ifdef VMS			/*****  VAX/VMS  *****/

extern double fabs();

#define VMS_MaxPrintableRealValue 1e240
#define siUTIL_PrepRealNumForPrinting(value)\
    ((fabs((double)value) > VMS_MaxPrintableRealValue)\
	? (int)FAULT_RecordSelectorNotFound ("Print OverFlow"),\
	  VMS_MaxPrintableRealValue\
	: (value))

#endif /* VMS */

#ifdef apollo			/*****  DOMAIN/IX  *****/

#define siUTIL_PrepRealNumForPrinting(value) value

#endif /* apollo */

#ifdef sun		/*****  Sun  *****/

#define siUTIL_PrepRealNumForPrinting(value) value

#endif /* sun */

#endif /* USEORIGINALPRINTF */

#endif /* BACKEND */

/***********************************
 *****  Searching and Sorting  *****
 ***********************************/
/*---------------------------------------------------------------------------
 *  Required Definitions:
 *	ADDRESS bsearch
 *	    (ADDRESS key, ADDRESS base,
 *	     unsigned nel, unsigned elsize,
 *	     int (*compar)())
 *				- see BSEARCH (3C)
 *
 *	void qsort
 *	    (ADDRESS base, unsigned nel, unsigned elsize, int (*compar)())
 *				- see QSORT (3C)
 *
 *	struct treenode {..., ADDRESS data; ... };
 *				- a structure defining the placement of the
 *				  datum within a tree node.
 *	tdata(node, type)  	- a macro accessing the datum within a tree
 *				  node given a pointer to the node.
 *	typedef enum { preorder, postorder, endorder, leaf } VISIT
 *				- the node visitation order of a 'twalk'.
 *	... tsearch (...)	- see TSEARCH (3C)
 *	... tfind (...)		- see TSEARCH (3C)
 *	... twalk (...)		- see TSEARCH (3C)
 *	... avlSearch (...)	- see "stdoss.c"
 *---------------------------------------------------------------------------
 */

/****************
 *  Invariants  *
 ****************/

PublicFnDecl ADDRESS
    bsearch ();

#if 0
#ifdef sun
#define qsort SUN_qsort
#endif
#endif

PublicFnDecl void
    qsort ();

PublicFnDecl struct treenode
    *tsearch (),
    *tfind ();
PublicFnDecl void
    twalk ();

#define tdata(node, type)	(type)((node)->data)

/********************
 *  Customizations  *
 ********************/

#ifdef hp9000s500		/*****  HP9000 Series 500  *****/

/*****  type declaration describes tree node to be built by "avlSearch" --
	note that the "key" field is the first one in the structure, as is
	the case with the UNIX "tsearch"
*****/
typedef struct treenode {
    ADDRESS 
    	data;
    struct treenode
    	*left, *right;
    short 
	bal;
} tnode;

#include <search.h>

PublicFnDecl struct treenode	/* replace UNIX tsearch... */
    *avlSearch ();
#define	 tsearch  avlSearch

#endif

#ifdef VMS			/*****  VAX/VMS  *****/
PublicFnDecl ADDRESS
    VMS_bsearch ();
PublicFnDecl void
    VMS_qsort ();

/**** Define bsearch and qsort to be their covers  ****/
#define bsearch VMS_bsearch
#define qsort VMS_qsort

struct treenode {
    struct treenode
    	*left, *right;
    short
    	reserved;
    ADDRESS 
    	data;
};

typedef enum { preorder, postorder, endorder, leaf } VISIT;

#endif

#ifdef apollo			/*****  DOMAIN/IX  *****/
/*****  type declaration describes tree node to be built by "avlSearch" --
	note that the "key" field is the first one in the structure, as is
	the case with the UNIX "tsearch"
*****/
typedef struct treenode {
    ADDRESS 
    	data;
    struct treenode
    	*left, *right;
    short 
	bal;
} tnode;

#include </sys5.3/usr/include/search.h>

PublicFnDecl struct treenode	/* replace UNIX tsearch... */
    *avlSearch ();
#define	 tsearch  avlSearch

#endif

#ifdef sun		/*****  Sun  *****/

/*****  type declaration describes tree node to be built by "avlSearch" --
	note that the "key" field is the first one in the structure, as is
	the case with the UNIX "tsearch"
*****/
typedef struct treenode {
    ADDRESS 
    	data;
    struct treenode
    	*left, *right;
    short 
	bal;
} tnode;

#include <search.h>

PublicFnDecl struct treenode	/* replace UNIX tsearch... */
    *avlSearch ();
#define	 tsearch  avlSearch

#endif


/*****************************
 *****  Memory Movement  *****
 *****************************/
/*---------------------------------------------------------------------------
 *  Required Definitions:
 *	memcpy			- See MEMORY(3C).
 *	memcpyb			- backward copying version of 'memcpy'.
 *	memset			- See MEMORY(3C).
 *---------------------------------------------------------------------------
 */

/****************
 *  Invariants  *
 ****************/

#ifdef sun
#define memcpy	SUN_memcpy
#endif

PublicFnDecl char
    *memcpy	(),
    *memset	();

/********************
 *  Customizations  *
 ********************/

#ifdef hp9000s500		/*****  HP9000 Series 500  *****/
PublicFnDecl char
    *memcpyb	();
#endif

#ifdef VMS			/*****  VAX/VMS  *****/
#define memcpyb memcpy
#define memcmp  strncmp
#endif

#ifdef apollo			/*****  DOMAIN/IX  *****/
#define memcpyb memcpy
#endif

#ifdef sun			/*****  Sun  *****/
PublicFnDecl char
    *memcpyb	();
#endif

/*******************************
 *****  Memory Allocation  *****
 *******************************/
/*---------------------------------------------------------------------------
 *  Required Definitions:
 *	char *malloc (int nbytes)
 *				- See MALLOC (3X)
 *	char *realloc (char *ptr, int nbytes)
 *				- See MALLOC (3X)
 *	char *calloc (int nitems, int size_of_item)
 *				- See MALLOC (3X)
 *	void free (char *ptr)	- See MALLOC (3X)
 *	void initmalloc ()	- a routine of void result and no arguments
 *				  that initializes the memory allocator in a
 *				  system dependent way.
 *	void querymalloc (struct MallocStats *stats)
 *				- a routine of void result and one argument -
 *				  the address of a 'MallocStats' structure.
 *				  Called to obtain memory allocator statistics.
 *---------------------------------------------------------------------------
 */

/****************
 *  Invariants  *
 ****************/

struct MallocStats {
    unsigned int
	total,
	used,
	free;
};

#ifndef lint
PublicFnDecl char
    *malloc	(),
    *realloc	(),
    *calloc	();
#endif /* lint */

PublicFnDecl void
    free	(),
    initmalloc	(),
    querymalloc	();

/********************
 *  Customizations  *
 ********************/

#ifdef hp9000s500		/*****  HP9000 Series 500  *****/
#endif

#ifdef VMS			/*****  VAX/VMS  *****/
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


/*****  Miscellaneaous  *****/

/****************
 *  Invariants  *
 ****************/

PublicFnDecl int	STD_execute();
PublicFnDecl void	STD_syserr(), STD_syswarn();

/********************
 *  Customizations  *
 ********************/

#ifdef hp9000s500		/*****  HP9000 Series 500  *****/
#define STD_NormalExitValue	0
#define STD_ErrorExitValue	1
#define STD_system(cmd)		RS_system (cmd)
#define	STD_pipe(fds,flags)	pipe(fds)
#endif

#ifdef VMS			/*****  VAX/VMS  *****/
/***** NOTE: SS$_NORMAL == STS$M_SUCCESS == 1 so simply use 1 here *****/
/***** since neither of these may be defined yet.		   *****/
#define STD_NormalExitValue	1
/***** NOTE: If the error value is 0 then a NONAME message is      *****/
/***** printed when the image exits in VMS.  Therefore, the value  *****/
/***** is set to 7 which is the maximum severity value and it does *****/
/***** not produce a message.  The DCL symbol $STATUS will contain *****/
/***** the exit value.  The DCL symbol $SEVERITY will contain the  *****/
/***** severity value (0-7).                                       *****/
#define STD_ErrorExitValue	7
PublicFnDecl			VMS_system ();
#define STD_system(cmd)		VMS_system ("")
#define execvp			execv
#define	STD_pipe(fds,flags)	pipe(fds,flags,4096)
#endif

#ifdef apollo			/*****  DOMAIN/IX  *****/
#define STD_NormalExitValue	0
#define STD_ErrorExitValue	1
#define STD_system(cmd)		RS_system (cmd)
#define	STD_pipe(fds,flags)	pipe(fds)
#endif

#ifdef sun		/*****  Sun  *****/
#define STD_NormalExitValue	0
#define STD_ErrorExitValue	1
#define STD_system(cmd)		RS_system (cmd)
#define	STD_pipe(fds,flags)	pipe(fds)
#endif

#endif

/***** Procedure used in preprocessors and some tools for fast	*****/
/***** conversion of strings to doubles				*****/
PublicFnDef double dtb();

/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  SIutil.h 1 replace /users/jck/system
  880330 21:58:47 jck Installing new standard interface

 ************************************************************************/
/************************************************************************
  SIutil.h 2 replace /users/jck/system
  880407 10:33:53 jck VAX modifications

 ************************************************************************/
/************************************************************************
  SIutil.h 3 replace /users/m2/dbup
  880421 17:18:48 m2 Apollo and VAX (and HP) port release

 ************************************************************************/
/************************************************************************
  SIutil.h 4 replace /users/m2/backend
  881205 10:40:42 m2 Lint fixes fo {m,c}alloc(), Standard exit values

 ************************************************************************/
/************************************************************************
  SIutil.h 5 replace /users/jck/system
  881207 09:43:17 jck Circumvent Apollo printf() limit

 ************************************************************************/
/************************************************************************
  SIutil.h 6 replace /users/m2/backend
  890503 14:59:23 m2 STD_printf() and IO_puts() changes

 ************************************************************************/
/************************************************************************
  SIutil.h 7 replace /users/m2/backend
  890927 14:41:39 m2 SUN port (preliminary)

 ************************************************************************/
