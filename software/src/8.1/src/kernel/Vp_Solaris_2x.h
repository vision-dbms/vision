#ifndef Vp_Solaris_2x_Interface
#define Vp_Solaris_2x_Interface

#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/systeminfo.h>

#include <dirent.h>

#include <float.h>
#include <ieeefp.h>
#include <inttypes.h>
#include <sunmath.h>

#include <search.h>
#include <unistd.h>

#define NaNQ quiet_nan (0)

#define _sysconf sysconf

#define INVALID_HANDLE_VALUE	-1

#define HANDLE			int
#define SOCKET			int

#define FileHandleIsValid(h)	(0 <= (h))
#define FileHandleIsInvalid(h)	(0 >  (h))

#define SystemHandleIsValid(h)	(0 <= (h))
#define SystemHandleIsInvalid(h)(0 >  (h))

#define MAXIMUM_WAIT_OBJECTS	FD_SETSIZE

#define gethostid		Vk_gethostid

#define __int64			long long
#define __int32			int
#define __int16			short
#define __int8			char

#define DECLSPEC_DLLEXPORT
#define DECLSPEC_DLLIMPORT

//  C++ Template Use Fixups
#define BOGUS_TEMPLATE template
#define BOGUS_TYPENAME typename


#endif
