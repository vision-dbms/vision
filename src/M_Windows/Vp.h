#ifndef Vp_Interface
#define Vp_Interface
#pragma once

//=====
//==  Headers
//=====

#include <winsock2.h>
#include <windows.h>

#include <sys/timeb.h>

#include <direct.h>
#include <float.h>
#include <io.h>
#include <limits.h>
#include <process.h>

//=====
//==  Types
//=====

//  type_info should be in std, but isn't in MS land.  Put it there...
namespace std {
    typedef ::type_info type_info;
}

typedef char*			caddr_t;
typedef unsigned short		mode_t;
typedef short			pid_t;

#ifdef _WIN64
typedef __int64			ssize_t;
#else
typedef int			ssize_t;
#endif

//  ISO <inttypes.h>
#define USING_STDINT
#ifndef USING_STDINT
typedef char			int8_t;
typedef short			int16_t;
typedef int			int32_t;
typedef __int64			int64_t;

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned __int64	uint64_t;

typedef __int64			intmax_t;
typedef unsigned __int64	uintmax_t;

typedef char			int_least8_t;
typedef short			int_least16_t;
typedef int			int_least32_t;
typedef __int64			int_least64_t;

typedef unsigned char		uint_least8_t;
typedef unsigned short		uint_least16_t;
typedef unsigned int		uint_least32_t;
typedef unsigned __int64	uint_least64_t;

typedef int			intptr_t;
typedef unsigned int		uintptr_t;

#define INT64_MAX		_I64_MAX
#define UINT64_MAX		_UI64_MAX
#else
#include <stdint.h>
#endif

//=====
//==  Defines
//=====

#ifdef _WIN64
#define _LP64
#endif

#define DECLSPEC_DLLEXPORT __declspec(dllexport)
#define DECLSPEC_DLLIMPORT __declspec(dllimport)

#if defined(_MSC_VER) && (_MSC_VER >= 1300)
#define USING_MSVC7
#else
#define USING_MSVC6
#endif

#define USING_HIDDEN_DEFAULT_VISIBILITY

#define NaNQ log (-1.0)

#define FileHandleIsValid(h)	(INVALID_HANDLE_VALUE != (h))
#define FileHandleIsInvalid(h)	(INVALID_HANDLE_VALUE == (h))

#define SystemHandleIsValid(h)	(NULL != (h))
#define SystemHandleIsInvalid(h)(NULL == (h))

#define alarm			Vk_alarm
#define atoll			_atoi64
#define chmod			_chmod
#define close			_close
#define creat(path,perm)	_open (\
    (path), _O_CREAT | _O_TRUNC | _O_RDWR | _O_BINARY, (perm)\
)
#define cuserid			Vk_cuserid
#define dup			_dup
#define dup2			_dup2
#define isfinite		_finite
#define fstat			_fstat
#define fsync			_commit
#define ftruncate		_chsize
#define getcwd			_getcwd
#define getegid()		0
#define geteuid()		0
#define getgid()		0
#define gethostid		Vk_gethostid
#define getpid			_getpid
#define getuid()		0
#define isnan			_isnan
#define lseek			_lseek
#define mkdir(path,perm)	_mkdir(path)
#define open(path,mode,perm)	_open ((path), (mode) | _O_BINARY, (perm))
#define pclose			_pclose
#define pipe(fds)		_pipe(fds,4096,_O_TEXT)
#define popen			_popen
#define read			_read
#define remove			Vk_remove
#define sleep			Vk_sleep
#define stat			_stat
#define strcasecmp		_stricmp
#define strncasecmp		_strnicmp
#define strtok_r		 strtok_s
#define umask			_umask
#define unlink			_unlink
#define write			_write

#ifndef va_copy
#define va_copy(dst,src) dst = (src)
#endif

//=====
//==  Pragmas
//=====

#pragma inline_depth (255)
#pragma warning (disable : 4231)
#pragma warning (disable : 4251)
#pragma warning (disable : 4273)
#pragma warning (disable : 4275)
#pragma warning (disable : 4291)
#pragma warning (disable : 4355)
#pragma warning (disable : 4660)
#pragma warning (disable : 4661)
#pragma warning (disable : 4786)
#pragma warning (disable : 4800)
#pragma warning (disable : 4996)

//=====
//==  Routines
//=====

#ifdef V_EXPORTS
#define Vp_V_API __declspec(dllexport)
#else
#define Vp_V_API __declspec(dllimport)
#endif

Vp_V_API char *asctime_r	(struct tm const *pTime, char *pBuffer);// ASSERT(sizeof (*pBuffer) >= 26);
Vp_V_API char *ctime_r		(time_t const *pTime, char *pBuffer);	// ASSERT(sizeof (*pBuffer) >= 26);
Vp_V_API struct tm *gmtime_r	(time_t const *pTime, struct tm *pResult);
Vp_V_API struct tm *localtime_r	(time_t const *pTime, struct tm *pResult);

#endif
