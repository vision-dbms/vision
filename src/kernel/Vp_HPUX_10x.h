#ifndef Vp_HPUX_10x_Interface
#define Vp_HPUX_10x_Interface

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/sem.h>
#include <sys/times.h>
#include <sys/utsname.h>

#include <dirent.h>
#include <float.h>
#include <search.h>
#include <syslog.h>
#include <unistd.h>

#define NaNQ log (-1.0)

#define INVALID_HANDLE_VALUE	-1

#define HANDLE			int
#define SOCKET			int

#define FileHandleIsValid(h)	(0 <= (h))
#define FileHandleIsInvalid(h)	(0 >  (h))

#define SystemHandleIsValid(h)	(0 <= (h))
#define SystemHandleIsInvalid(h)(0 >  (h))

#define MAXIMUM_WAIT_OBJECTS	FD_SETSIZE

#define gethostid		Vk_gethostid

#define finite			Vk_finite

#endif
