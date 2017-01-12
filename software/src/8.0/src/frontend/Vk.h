/*****  Vision Kernel Interface  *****/
#ifndef Vk_Interface
#define Vk_Interface

/*****************************************
 *****************************************
 *****  Operating System Interfaces  *****
 *****************************************
 *****************************************/

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <stdarg.h>

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "Vp.h"


/*****************************
 *****************************
 *****  Storage Classes  *****
 *****************************
 *****************************/

/*---------------------------------------------------------------------------
 *  Required Definitions:
 *
 *	PublicVarDef		- 'extern' variable definition.
 *	PublicVarDecl		- 'extern' variable declaration.
 *	PrivateVarDef		- 'static' variable definition.
 *
 *	PublicFnDef		- 'extern' function definition.
 *	PublicFnDecl		- 'extern' function declaration.
 *	PrivateFnDef		- 'static' function definition.
 *
 *---------------------------------------------------------------------------
 */

#define PublicVarDef
#define PublicVarDecl		extern
#define PrivateVarDef		static

#define PublicFnDef
#define PublicFnDecl		extern
#define PrivateFnDef		static


/****************************
 ****************************
 *****  Standard Types  *****
 ****************************
 ****************************/

typedef unsigned long		VkCount;
typedef unsigned long		VkSize;


/*******************************************************
 *******************************************************
 *****  Standard Macros, Variables, and Constants  *****
 *******************************************************
 *******************************************************/

/*---------------------------------------------------------------------------
 *  Required Definitions:
 *
 *	MIN(a,b)		- self-explanatory
 *	MAX(a,b)		- self-explanatory
 *	TRUE			- self-explanatory
 *	FALSE			- self-expanatory
 *	NormalExitValue		- the value returned by a 'main' program to
 *				  indicate normal termination.
 *	ErrorExitValue		- the value returned by a 'main' program to
 *				  indicate abnormal termination or an abnormal
 *				  execution event.
 *      Unused(p)               - macro used to wrap unused parameter names in
 *                                order to keep certain compilers (HP-UX C++)
 *                                happy.
 *
 *	sys_nerr		- number of OS reportable errors.
 *	sys_errlist		- array  of OS reportable error descriptions.
 *
 *---------------------------------------------------------------------------
 */

#ifndef MIN
#define	MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define	MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define NormalExitValue	0
#define ErrorExitValue	1

#if defined(__hp9000s700) && defined(__cplusplus)

#define Unused(p)
#else

#define Unused(p) p
#endif

#ifndef __APPLE__
extern int sys_nerr;
#endif


/**************************
 **************************
 *****  Data Formats  *****
 **************************
 **************************/

#define Vk_DataFormatBEndian	0
#define Vk_DataFormatLEndian	1

#define Vk_DataFormatChanging	0xff


/****************************************************************
 ****************************************************************
 *****  Machine Dependent Macros, Variables, and Constants  *****
 ****************************************************************
 ****************************************************************/

/*---------------------------------------------------------------------------
 *  Required Definitions:
 *
 *	OffsetOfNthByte(type,n)	- the offset of the n-th least significant byte
 *				  of a byte-flippable data type (n==1 <==> LSB)
 *	MAXINTDIGITS		- the maximum number of digits in an Integer.
 *	MAXDOUBLEDIGITS		- the maximum number of whole digits in a
 *                                Double.
 *	MAXDOUBLESIGDIGITS	- the number of significant digits in a Double.
 *	Vk_DataFormatNative	- the binary data format for this processor
 *				  architecture, chosen from among the choices in
 *				  the data formats section of this file.
 *	Vk_DataFormatReversed	- the binary data format of the bit reversal of
 *				  this processor architecture, chosen from among
 *				  the choices in the data formats section of this
 *				  file.
 *
 *  Note:
 *	These architecture dependent definitions could all migrate to the
 *	appropriate 'Vp_*.h' file.
 *
 *---------------------------------------------------------------------------
 */

/*******************
 *****  Intel  *****
 *******************/

#if defined(_WIN32)

#define MAXINTDIGITS		11
#define MAXDOUBLEDIGITS		309
#define MAXDOUBLESIGDIGITS	16

#define OffsetOfNthByte(type, n)((n) - 1)

#define Vk_DataFormatNative	Vk_DataFormatLEndian
#define Vk_DataFormatReversed	Vk_DataFormatBEndian

/*******************
 *****  HP-PA  *****
 *******************/

#elif defined(__hp9000s700)
				/*****  HP9000  *****/

#define MAXINTDIGITS		11
#define MAXDOUBLEDIGITS		309
#define MAXDOUBLESIGDIGITS	16

#define OffsetOfNthByte(type, n)(sizeof (type) - (n))

#define Vk_DataFormatNative	Vk_DataFormatBEndian
#define Vk_DataFormatReversed	Vk_DataFormatLEndian

/*******************
 *****  SPARC  *****
 *******************/

#elif defined (sun)
				/*****  Sun  *****/

#define MAXINTDIGITS		11
#define MAXDOUBLEDIGITS		308
#define MAXDOUBLESIGDIGITS	16

#define OffsetOfNthByte(type, n)(sizeof (type) - (n))

#define Vk_DataFormatNative	Vk_DataFormatBEndian
#define Vk_DataFormatReversed	Vk_DataFormatLEndian


/*****************
 *****  AIX  *****
 *****************/

#elif defined(_AIX)
				/*****  AIX  *****/

#define MAXINTDIGITS		11
#define MAXDOUBLEDIGITS		309
#define MAXDOUBLESIGDIGITS	16

#define OffsetOfNthByte(type, n)(sizeof (type) - (n))

#define Vk_DataFormatNative	Vk_DataFormatBEndian
#define Vk_DataFormatReversed	Vk_DataFormatLEndian

#endif


/*********************************************
 *********************************************
 *****  'Nil' Definition and Predicates  *****
 *********************************************
 *********************************************/

/*---------------------------------------------------------------------------
 *  Required Definitions:
 *	NilOf(pointerType)	- the 'Nil' of 'pointerType'
 *	IsNil(pointerValue)	- 'True' if 'pointerValue' is 'Nil'
 *	IsntNil(pointerValue)	- 'True' if 'pointerValue' is not 'Nil'
 *
 *---------------------------------------------------------------------------
 */
#define NilOf(type)		((type)0)
#define IsNil(pointer)		((void *)(pointer) == NilOf (void *))
#define IsntNil(pointer)	((void *)(pointer) != NilOf (void *))


/*******************************
 *******************************
 *****  Memory Allocation  *****
 *******************************
 *******************************/

#define VkAllocator		struct VkAllocator

VkAllocator {
    void *			(*m_pAllocateImplementation	)(size_t);
    void *			(*m_pReallocateImplementation	)(void *, size_t);
    void			(*m_pFreeImplementation	)(void *);
};

PublicVarDecl VkAllocator	VkAllocator_DefaultInstance;

#define Vk_Allocate(s)		VkAllocator_DefaultInstance.m_pAllocateImplementation(s)
#define Vk_Reallocate(p,s)	VkAllocator_DefaultInstance.m_pReallocateImplementation((p),(s))
#define Vk_Deallocate(p)	VkAllocator_DefaultInstance.m_pFreeImplementation(p)

PublicFnDecl void VkAllocator_Initialize (
    VkAllocator*		pThis,
    void *			(*pAllocateImplementation	)(size_t),
    void *			(*pReallocateImplementation	)(void *, size_t),
    void			(*pFreeImplementation		)(void *)
);


/************************************
 ************************************
 *****  Timers and Time Stamps  *****
 ************************************
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

/*****  Timer  *****/
struct timer {
    unsigned long		elapsed;
    unsigned long		user;
    unsigned long		system;
    unsigned long		cuser;
    unsigned long		csystem;
};

/*****  Time  *****/
struct time {
    unsigned long		seconds;
    unsigned long		microseconds;
};

PublicFnDecl int gettimer (
    struct timer		*ptimer
);

PublicFnDecl int gettime (
    struct time			*ptime
);


/**********************************
 **********************************
 *****  Formatting Utilities  *****
 **********************************
 **********************************/

/*---------------------------------------------------------------------------
 *  Required Definitions:
 *	STD_printf	(see printf).
 *	STD_fprintf	(see fprintf).
 *	STD_sprintf	(see sprintf).
 *	STD_vprintf	(see vprintf).
 *	STD_vfprintf	(see vfprintf).
 *	STD_vsprintf	(see vsprintf).
 *	STD_sscanf	(see sscanf).
 *---------------------------------------------------------------------------
 */

#define STD_printf	printf
#define STD_vprintf	vprintf

#define STD_fprintf	fprintf
#define STD_vfprintf	vfprintf

#if defined(solaris_1)
PublicFnDecl int STD_sprintf(
    char *			buffer,
    const char *		format,
    ...
);

PublicFnDecl int STD_vsprintf(
    char *			buffer,
    const char *		format,
    va_list			ap
);

#else
#define STD_sprintf	sprintf
#define STD_vsprintf	vsprintf
#endif

#if defined(__hp9000s700)
PublicFnDecl int STD_sscanf (
    const char *		source,
    const char *		format,
    ...
);

#else
#define STD_sscanf	sscanf
#endif


/************************************
 ************************************
 *****  Byte Array Re-Ordering  *****
 ************************************
 ************************************/

PublicFnDecl void Vk_ReverseArray (
    void *			pArray,
    size_t			sArray
);

#define Vk_ReverseSizeable(pField) Vk_ReverseArray ((pField), sizeof (*(pField)))


/********************************************
 ********************************************
 *****  Balanced Binary Tree Utilities  *****
 ********************************************
 ********************************************/

/*---------------------------------------------------------------------------
 *  Required Definitions:
 *
 *	struct treenode {..., ADDRESS data; ... };
 *				- a structure defining the placement of the
 *				  datum within a tree node.
 *	tdata(node, type)  	- a macro accessing the datum within a tree
 *				  node given a pointer to the node.
 *	typedef enum {
 *	    STD_preorder, STD_postorder, STD_endorder, STD_leaf
 *	} STD_VISIT
 *				- the node visitation order of a 'twalk'.
 *	... STD_tsearch (...)	- see TSEARCH (3C)
 *	... STD_tfind (...)	- see TSEARCH (3C)
 *	... STD_twalk (...)	- see TSEARCH (3C)
 *
 *---------------------------------------------------------------------------
 */

typedef struct treenode {
    void const *		data;
    struct treenode		*left, *right;
    short			bal;
} tnode;

#define STD_tdata(node)		((node)->data)

PublicFnDecl struct treenode *STD_tsearch (
    void const *		key,
    void **			rootp,
    int				(*compar)(void const *, void const *)
);

PublicFnDecl struct treenode *STD_tfind (
    void const *		key,
    void **			rootp,
    int				(*compar)(void const *, void const *)
);

typedef enum {
    STD_preorder, STD_postorder, STD_endorder, STD_leaf
} STD_VISIT;

PublicFnDecl void STD_twalk (
    void const *		rootp,
    void			(*action)(struct treenode *, STD_VISIT, int)
);


/********************************
 ********************************
 *****  Option Acquisition  *****
 ********************************
 ********************************/

#if defined(_WIN32)
PublicFnDecl int getopt (
    int				argc,
    char			**argv,
    char			*optstring
);
#endif


/*****************************
 *****************************
 *****  Memory Movement  *****
 *****************************
 *****************************/

/*---------------------------------------------------------------------------
 *  Required Definitions:
 *	memcpy			- See MEMORY(3C).
 *	memcpyb			- backward copying version of 'memcpy'.
 *	memset			- See MEMORY(3C).
 *---------------------------------------------------------------------------
 */

#if defined(_WIN32) || defined(__hp9000s700) || defined(solaris_2) || defined(_AIX) || defined(__linux__)

				/*****  AIX  *****/
				/*****  HP9000 Series 700  *****/
				/*****  Solaris 2.x  *****/
				/*****  Windows/NT  *****/
#define memcpy	memmove
#define memcpyb	memmove

#elif defined(solaris_1)
				/*****  Solaris 1.x  *****/
PublicFnDecl void *STD_memmove (
    void *			dst,
    const void *		src,
    unsigned int		n
);

#define memcpy	STD_memmove
#define memcpyb	STD_memmove
#endif


/******************************
 ******************************
 *****  Directory Access  *****
 ******************************
 ******************************/

/************************
 *****  Windows/NT  *****
 ************************/

#if defined(_WIN32)

#define DIR			struct STD_DirectoryCursor
typedef char			STD_DirectoryEntryType;

#define STD_DirectoryEntryName(dirent)		(dirent)
#define STD_DirectoryEntryNameLen(dirent)	strlen (dirent)

PublicFnDecl DIR *opendir (
    char 			*directoryName
);

PublicFnDecl STD_DirectoryEntryType *readdir (
    DIR				*directoryCursor
);

PublicFnDecl void closedir (
    DIR				*directoryCursor
);


/*******************************
 *****  AIX / HP-UX / Sun  *****
 *******************************/

#elif defined(_AIX) || defined(__hp9000s700) || defined(sun) || defined(__linux__)

typedef struct dirent STD_DirectoryEntryType;

#define STD_DirectoryEntryName(dirent)	 ((dirent)->d_name)

#if defined(solaris_2)
#define STD_DirectoryEntryNameLen(dirent)strlen ((dirent)->d_name)
#else
#define STD_DirectoryEntryNameLen(dirent)((dirent)->d_namlen)
#endif
#endif


/*****************************
 *****************************
 *****  Signal Handling  *****
 *****************************
 *****************************/

/************************
 *****  Windows/NT  *****
 ************************/

#if defined(_WIN32)

#define SIGPIPE		0
#define SIGHUP		1
#define SIGQUIT		22
#define SIGTRAP		5
#define SIGIOT		6
#define SIGEMT		7
#define SIGKILL		9
#define SIGBUS		10
#define SIGSYS		12
#define SIGALRM		13
#define SIGUSR1		16
#define SIGUSR2		17
#define SIGCLD		18
#define SIGPWR		19
#define SIGVTALRM	20
#define SIGPROF		21
#define SIGIO		23
#define SIGWINDOW	24
#define SIGAPOLLO	25
#define SIGTSTP		26
#define SIGCONT		27
#define SIGCHLD		SIGCLD
#define SIGTTIN		28
#define SIGTTOU		29
#define SIGXCPU		30
#define SIGXFSZ		31
#define SIGURG		32
#define SIGWINCH	33

#define STD_maskType		int

typedef struct sigcontext *STD_sigcontext;

typedef void (*STD_SignalHandlerType) (
    int				sig,
    int				code,
    struct sigcontext		*scp
);

struct sigvec {
    STD_SignalHandlerType	sv_handler;
    int				sv_mask;
    int				sv_flags;
    int				sv_onstack;
};

PublicFnDecl STD_maskType STD_sigblock (
    STD_maskType		bitmap
);

PublicFnDecl STD_maskType STD_sigsetmask (
    STD_maskType		bitmap
);

PublicFnDecl int STD_sigvector (
    int				sig,
    struct sigvec		*ivec,
    struct sigvec		*ovec
);

#define STD_setRestartAfterSignal(contextStruct) /*** No op  ***/


/*******************************
 *****  HP9000 Series 700  *****
 *******************************/

#elif defined(__hp9000s700)

/****  Define Apollo used signals (these will not occur on HP)  ****/
#define SIGAPOLLO	40	/* Apollo-specific fault */
#define SIGXCPU		41	/* cpu time limit exceeded */
#define SIGXFSZ		42	/* file size limit exceeded */

#define STD_maskType		long

#ifdef __cplusplus
typedef void (*STD_SignalHandlerType) (
    int				sig
);

#else
typedef void (*STD_SignalHandlerType) (
    int				sig,
    int				code,
    struct sigcontext		*scp
);

#endif

#define STD_sigvector		sigvector
#define STD_sigblock		sigblock
#define STD_sigsetmask		sigsetmask

#define STD_setRestartAfterSignal(contextStruct)\
{\
    if (contextStruct->sc_syscall != SYS_NOTSYSCALL &&\
        contextStruct->sc_syscall != SYS_PAUSE)\
        contextStruct->sc_syscall_action = SIG_RESTART;\
}


/*****************************
 *****  AIX and Solaris  *****
 *****************************/

#elif defined(_AIX) || defined(sun) || defined(__linux__)

/*****  Define names for HP signals  *****/
#if defined(solaris_1)
#define	SIGPWR	  40	/* Apollo replaced SIGPWR with SIGAPOLLO */
#endif
#define	SIGWINDOW 41 	/* Apollo doesn't have SIGWINDOW */

/****  Define Apollo used signals (these will not occur on Sun)  ****/
#define SIGAPOLLO 42   /* Apollo-specific fault */

#define STD_maskType		int

typedef struct sigcontext *STD_sigcontext;

typedef void (*STD_SignalHandlerType) (
    int				sig,
    int				code,
    struct sigcontext		*scp
);

#if defined(solaris_2) || defined(__linux__) && !defined(__USE_BSD)
struct sigvec {
    STD_SignalHandlerType	sv_handler;
    int				sv_mask;
    int				sv_flags;
    int				sv_onstack;
};
#endif

PublicFnDecl STD_maskType STD_sigblock (
    STD_maskType		bitmap
);

PublicFnDecl STD_maskType STD_sigsetmask (
    STD_maskType		bitmap
);

PublicFnDecl int STD_sigvector (
    int				sig,
    struct sigvec		*ivec,
    struct sigvec		*ovec
);

#define STD_setRestartAfterSignal(contextStruct) /*** No op  ***/

#endif /* AIX/Solaris/Linux */



/*******************
 *******************
 *****  Other  *****
 *******************
 *******************/

/************************
 *****  Windows/NT  *****
 ************************/

#if defined(_WIN32)
PublicFnDecl int Vk_alarm (
    size_t			sInterval
);

PublicFnDecl char* Vk_cuserid (
    char*			pBuffer
);

PublicFnDecl int Vk_link (
    const char*			pSrcPath,
    const char*			pDstPath
);

PublicFnDecl void Vk_remove (
    const char*			pFileName
);

PublicFnDecl size_t Vk_sleep (
    size_t			sSleepInterval
);
#endif

#endif
