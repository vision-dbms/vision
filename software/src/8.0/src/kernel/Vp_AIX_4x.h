#ifndef Vp_AIX_4x_Interface
#define Vp_AIX_4x_Interface

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/lockf.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/times.h>
#include <sys/utsname.h>

#include <dirent.h>
#include <float.h>
#include <libgen.h>
#include <search.h>
#include <strings.h>
#include <unistd.h>

#define NaNQ			DBL_QNAN

#define vfork 			fork

#define INVALID_HANDLE_VALUE	-1

#define HANDLE			int
#define SOCKET			int

#define FileHandleIsValid(h)	(0 <= (h))
#define FileHandleIsInvalid(h)	(0 >  (h))

#define SystemHandleIsValid(h)	(0 <= (h))
#define SystemHandleIsInvalid(h)(0 >  (h))

#define MAXIMUM_WAIT_OBJECTS	FD_SETSIZE

#ifdef __cplusplus

#define F_ULOCK 0       /* Unlock a previously locked region */
#define F_LOCK  1       /* Lock a region for exclusive use */
#define F_TLOCK 2       /* Test and lock a region for exclusive use */
#define F_TEST  3       /* Test region for other processes locks */

extern "C" int lockf (int, int, off_t);

#endif /* __cplusplus */

#endif
