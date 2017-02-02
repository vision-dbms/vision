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

#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

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
#include <typeinfo>

#include "Vp.h"

/*********************
 *********************
 *****  Library  *****
 *********************
 *********************/

#include "V.h"


/*****************************************************
 *****************************************************
 *****  Storage Classes and Linkage Conventions  *****
 *****************************************************
 *****************************************************/

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
 *	INLINE_TMEMBER		- 'inline' or empty for platform specific
 *				  template member inlining.  Define in Vp.h
 *				  as appropriate.
 *	UNALIGNED		- a macro used to qualify a pointer to a type
 *				  to indicate that the pointer may not be
 *				  pointing to a naturally aligned (multiple
 *				  of sizeof(Type)) instance of the type.
 *	USING(name)		- a macro for inserting optional 'using name;'
 *				  declarations in 'strict' ansi code.
 *
 *	BOGUS_TEMPLATE		- 'template' or empty for platform specific
 *				  bogus error suppression.  Defined in Vp.h
 *				  as appropriate.
 *	BOGUS_TYPENAME		- 'typename' or empty for platform specific
 *				  bogus error suppression.  Defined in Vp.h
 *				  Vp.h as appropriate.
 *	DECLSPEC_DLLEXPORT	- __declspec(dllexport) or its equivalent on
 *				  platforms that support it.
 *	DECLSPEC_DLLIMPORT	- __declspec(dllimport) or its equivalent on
 *				  platforms that support it.
 *---------------------------------------------------------------------------
 */

#define PublicVarDef
#define PublicVarDecl		extern
#define PrivateVarDef		static

#define PublicFnDef
#define PublicFnDecl		extern
#define PrivateFnDef		static

#ifndef INLINE_TMEMBER
#define INLINE_TMEMBER
#endif

#ifndef UNALIGNED
#define UNALIGNED
#else
#define HAS_UNALIGNED
#endif

#ifndef USING
#define USING(name)
#endif

#ifndef BOGUS_TEMPLATE
#define BOGUS_TEMPLATE
#endif

#ifndef BOGUS_TYPENAME
#define BOGUS_TYPENAME
#endif

#ifndef _WIN32
#define __cdecl
#endif

#ifndef DECLSPEC_DLLEXPORT
#define DECLSPEC_DLLEXPORT
#endif

#ifndef DECLSPEC_DLLIMPORT
#define DECLSPEC_DLLIMPORT
#endif


/****************************
 ****************************
 *****  Standard Types  *****
 ****************************
 ****************************/

#ifdef __VMS
#pragma __pointer_size __save
#pragma __pointer_size 32
#endif
typedef char **argv_t;
#ifdef __VMS
#pragma __pointer_size __restore
#endif

typedef char *pointer_t;

#ifdef __VMS
typedef __int64 pointer_diff_t;
#else
typedef ptrdiff_t pointer_diff_t;
#endif

#if defined(_LP64) || defined(_M_X64)
typedef unsigned __int64 pointer_size_t;
#else
typedef unsigned __int32 pointer_size_t;
#endif

extern "C" {
    typedef int (__cdecl *VkComparator)(void const *, void const *);
}


/*******************************************************
 *******************************************************
 *****  Standard Macros, Variables, and Constants  *****
 *******************************************************
 *******************************************************/

/*---------------------------------------------------------------------------
 *  Required Definitions:
 *
 *	V_Min(a,b)		- self-explanatory
 *	V_Max(a,b)		- self-explanatory
 *	NormalExitValue		- the value returned by a 'main' program to
 *				  indicate normal termination.
 *	ErrorExitValue		- the value returned by a 'main' program to
 *				  indicate abnormal termination or an abnormal
 *				  execution event.
 *      Unused(p)               - macro used to wrap unused parameter names in
 *                                order to keep certain compilers (HP-UX C++)
 *                                happy.
 *
 *---------------------------------------------------------------------------
 */
#ifndef V_Min
#define	V_Min(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef V_Max
#define	V_Max(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef NormalExitValue
#define NormalExitValue	V::ExitCode ()
#endif

#ifndef ErrorExitValue
#define ErrorExitValue	V::ExitErrorCode ()
#endif

namespace V {
    int ExitCode ();
    int ExitErrorCode ();
}

#if defined(__hp9000s700)
#define Unused(p)
#else
#define Unused(p) p
#endif

#ifndef ENTER_DEBUGGER
#define ENTER_DEBUGGER()
#endif

#ifndef ENTER_DEBUGGER_IF
#define ENTER_DEBUGGER_IF(ignore)
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
 *	INT64_MAX		- the largest 64-bit signed integer.
 *	UINT64_MAX		- the largest 64-bit unsigned integer.
 *
 *  Note:
 *	These architecture dependent definitions could all migrate to the
 *	appropriate 'Vp_*.h' file.
 *
 *---------------------------------------------------------------------------
 */

#ifndef INT64_MAX
#define INT64_MAX  0x7fffffffffffffffLL
#endif

#ifndef UINT64_MAX
#define UINT64_MAX 0xffffffffffffffffULL
#endif

#define S64_MAX INT64_MAX
#define U64_MAX UINT64_MAX

/*******************
 *****  Intel  *****
 *******************/

#if defined(__VMS) || defined(_M_IX86) || defined(_M_X64) || defined(__i386) || defined(__i386__) || defined (__x86_64__) || defined (__arm__)

#define MAXINTDIGITS		11
#define MAXDOUBLEDIGITS		309
#define MAXDOUBLESIGDIGITS	16

#define OffsetOfNthByte(type, n)((n) - 1)

#define Vk_DataFormatNative	Vk_DataFormatLEndian
#define Vk_DataFormatReversed	Vk_DataFormatBEndian

/*******************
 *****  HP-PA  *****
 *******************/

#elif defined(__hp9000s800) || defined(__hp9000s700)
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

#elif defined (__sparc)
				/*****  Sun/Sparc  *****/

#define MAXINTDIGITS		11
#define MAXDOUBLEDIGITS		309
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
#define IsNil(pointer)		((pointer) ? false : true)
#define IsntNil(pointer)	((pointer) ? true : false)


/*************************
 *************************
 *****  Time Stamps  *****
 *************************
 *************************/

/*---------------------------------------------------------------------------
 *  Required Definitions:
 *	struct time		- a time stamp.
 *				  A timestamp is assumed to record the system's
 *				  current notion (GMT?) of the absolute time in
 *				  microseconds (subject to system clock
 *				  granularity).
 *	int gettime (struct time *time)
 *				- routine to access the current absolute time.
 *				  Returns 0 if successful; -1 and 'errno' if
 *				  not.
 *---------------------------------------------------------------------------
 */

struct time {
    unsigned int seconds;
    unsigned int microseconds;
};

PublicFnDecl V_API int gettime (struct time *ptime);


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

#define STD_sprintf	sprintf
#define STD_vsprintf	vsprintf

#if defined(__hp9000s700)
PublicFnDecl int STD_sscanf (char const *source, char const *format, ...);
#else
#define STD_sscanf	sscanf
#endif


/************************************
 ************************************
 *****  Byte Array Re-Ordering  *****
 ************************************
 ************************************/

PublicFnDecl V_API void Vk_ReverseArray (void *pElements, size_t sElement, size_t cElements = 1);

#define Vk_ReverseSizeable(pField) Vk_ReverseArray ((pField), sizeof (*(pField)))


/********************************************
 ********************************************
 *****  Balanced Binary Tree Utilities  *****
 ********************************************
 ********************************************/

/*---------------------------------------------------------------------------
 *  Required Definitions:
 *
 *	treenode {..., ADDRESS data; ... };
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

struct treenode {
    typedef short bal_t;
    void const *		data;
    struct treenode		*left, *right;
    bal_t			bal;
};

#define STD_tdata(node)		((node)->data)

PublicFnDecl V_API treenode *STD_tsearch (
    void const *		key,
    void **			rootp,
    VkComparator		comparator
);

PublicFnDecl V_API treenode *STD_tfind (
    void const *		key,
    void **			rootp,
    VkComparator		comparator
);

enum STD_VISIT {
    STD_preorder, STD_postorder, STD_endorder, STD_leaf
};

PublicFnDecl V_API void STD_twalk (
    void const *		rootp,
    void			(*action)(treenode *, STD_VISIT, int)
);


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

#ifndef memcpy
#define memcpy	memmove
#endif

#define memcpyb	memmove


/******************************
 ******************************
 *****  Directory Access  *****
 ******************************
 ******************************/

/*********************
 *****  Windows  *****
 *********************/

#if defined(_WIN32)

struct DIR;
typedef char STD_DirectoryEntryType;

#define STD_DirectoryEntryName(dirent)		(dirent)
#define STD_DirectoryEntryNameLen(dirent)	strlen (dirent)

PublicFnDecl V_API DIR *opendir (char const*directoryName);

PublicFnDecl V_API STD_DirectoryEntryType* readdir (DIR *directoryCursor);

PublicFnDecl V_API void closedir (DIR* directoryCursor);


/******************
 *****  Unix  *****
 ******************/

#else

typedef struct dirent STD_DirectoryEntryType;

#define STD_DirectoryEntryName(dirent)	 ((dirent)->d_name)
#define STD_DirectoryEntryNameLen(dirent)strlen ((dirent)->d_name)

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

PublicFnDecl V_API STD_maskType STD_sigblock (
    STD_maskType		bitmap
);

PublicFnDecl V_API STD_maskType STD_sigsetmask (
    STD_maskType		bitmap
);

PublicFnDecl V_API int STD_sigvector (
    int				sig,
    struct sigvec		*ivec,
    struct sigvec		*ovec
);

#define STD_setRestartAfterSignal(contextStruct) /*** No op  ***/


/*******************************
 *****  HP9000 Series 700  *****
 *******************************/

#elif defined(__hp9000s800) || defined(__hp9000s700)

/****  Define Apollo used signals (these will not occur on HP)  ****/
#if !defined (__hp9000s800)
#define SIGXCPU		41	/* cpu time limit exceeded */
#define SIGXFSZ		42	/* file size limit exceeded */
#endif

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

#define STD_setRestartAfterSignal(contextStruct) {\
    if (contextStruct->sc_syscall != SYS_NOTSYSCALL &&\
        contextStruct->sc_syscall != SYS_PAUSE)\
        contextStruct->sc_syscall_action = SIG_RESTART;\
}


/*******************************
 *****  AIX/Linux/Solaris  *****
 *******************************/

#elif defined(__VMS) || defined(_AIX) || defined(__linux__) || defined(sun) || defined(__APPLE__)

/*****  Define names for unsupported signals  *****/
#if defined(__linux__)
#define SIGEMT SIGUNUSED+1 /* Linux doesn't have this one */
#define SIGWINDOW SIGUNUSED+2 /* Linux doesn't have this one */

#elif defined(__APPLE__)
#define SIGCLD SIGCHLD

#else
#define	SIGWINDOW 41

#endif

#define STD_maskType		int

typedef struct sigcontext *STD_sigcontext;

#if defined(sun)
typedef void (*STD_SignalHandlerType) (
    int				sig,
    int				code,
    struct sigcontext		*scp
);
#else
extern "C" {
typedef void (*STD_SignalHandlerType)(int);
}
#endif

#if defined(sun) || defined(__linux__) && !defined(__USE_BSD)
struct sigvec {
    STD_SignalHandlerType	sv_handler;
    int				sv_mask;
    int				sv_flags;
    int				sv_onstack;
};
#endif

PublicFnDecl V_API STD_maskType STD_sigblock (
    STD_maskType		bitmap
);

PublicFnDecl V_API STD_maskType STD_sigsetmask (
    STD_maskType		bitmap
);

PublicFnDecl V_API int STD_sigvector (
    int				sig,
    struct sigvec		*ivec,
    struct sigvec		*ovec
);

#define STD_setRestartAfterSignal(contextStruct) /*** No op  ***/

#endif /* AIX/Linux/Solaris  */


/***************************
 ***************************
 *****  Host and User  *****
 ***************************
 ***************************/

PublicFnDecl V_API long Vk_gethostid ();

PublicFnDecl V_API char const *Vk_username (char *pBuffer, size_t sBuffer);


/**********************
 **********************
 *****  'finite'  *****
 **********************
 **********************/

#if defined(__hp9000s800) || defined(__hp9000s700)
PublicFnDecl int Vk_finite (double x);
#endif


/*******************
 *******************
 *****  Other  *****
 *******************
 *******************/

/************************
 *****  Windows/NT  *****
 ************************/

#if defined(_WIN32)

PublicFnDecl V_API int    Vk_alarm	(size_t sInterval);

PublicFnDecl V_API void   Vk_remove	(char const *pFileName);

PublicFnDecl V_API size_t Vk_sleep	(size_t sSleepInterval);

#endif


#endif
