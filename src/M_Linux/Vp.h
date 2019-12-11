#ifndef Vp_Linux_Interface
#define Vp_Linux_Interface

#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/times.h>

#include <dirent.h>

#include <float.h>
#include <ieee754.h>
#include <math.h>

#include <search.h>
#include <stdint.h>
#include <unistd.h>

#include "VConfig.h"
#ifdef VMS_LINUX_EXPLICIT_COMPAT
#include "basics.h"
#include "vms_compat.h"
#define INLINE_TMEMBER inline
#ifndef USING_HIDDEN_DEFAULT_VISIBILITY
#define USING_HIDDEN_DEFAULT_VISIBILITY
#endif
#endif

#define NaNQ log(-1.0)

#define _sysconf sysconf

#define INVALID_HANDLE_VALUE	-1

#define HANDLE			int
#define SOCKET			int

#define FileHandleIsValid(h)	(0 <= (h))
#define FileHandleIsInvalid(h)	(0 >  (h))

#define SystemHandleIsValid(h)	(0 <= (h))
#define SystemHandleIsInvalid(h)(0 >  (h))

#define MAXIMUM_WAIT_OBJECTS	FD_SETSIZE

#define USING(x) using x;

#ifndef DECLSPEC_DLLEXPORT
#define DECLSPEC_DLLEXPORT
#endif

#ifndef DECLSPEC_DLLIMPORT
#define DECLSPEC_DLLIMPORT
#endif

#ifndef __int64
#define __int64			long long
#endif
#ifndef __int32
#define __int32			int
#endif
#ifndef __int16
#define __int16			short
#endif
#ifndef __int8
#define __int8			char
#endif

#endif
