#ifndef Vp_Interface
#define Vp_Interface

//=====
//==  Headers
//=====

#include <sys/ioctl.h>
#include <sys/times.h>
#include <time.h>

#include <dirent.h>

#include <float.h>
#include <math.h>

#include <unistd.h>

#include "basics.h"

//=====
//==  Pragmas
//=====

#pragma message disable(INSTENTITY,MAINMANYPRM,BASCLSNONDTO,NOSTDLONGLONG)

//=====
//==  Types
//=====

#define INT64_MAX __INT64_MAX
#define UINT64_MAX __UINT64_MAX

//=====
//==  Defines
//=====

#include "VConfig.h"

#define UNALIGNED __unaligned
#define USING(name) using name;

#ifdef __VMS
#define _LP64
#endif

#define INLINE_TMEMBER inline

#define NaNQ log(-1.0)

#define gethostid Vk_gethostid

#define INVALID_HANDLE_VALUE	-1

#define HANDLE			int
#define SOCKET			int

#define FileHandleIsValid(h)	(0 <= (h))
#define FileHandleIsInvalid(h)	(0 >  (h))

#define SystemHandleIsValid(h)	(0 <= (h))
#define SystemHandleIsInvalid(h)(0 >  (h))

#define MAXIMUM_WAIT_OBJECTS	FD_SETSIZE


#define fflush(filestream_ptr) \
        do { fflush(filestream_ptr); \
        fsync(fileno (filestream_ptr)); } while(0)

#ifdef __VMS
#define va_copy(dst,src) dst = (src)
#endif


#endif
