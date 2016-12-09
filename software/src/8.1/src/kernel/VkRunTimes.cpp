/*****  VkRunTimes Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VkRunTimes.h"

/************************
 *****  Supporting  *****
 ************************/

#include "V_VTime.h"

#ifdef __VMS
#ifndef __NEW_STARLET
#define __NEW_STARLET
#endif
#ifndef _LARGEFILE
#define _LARGEFILE
#endif

#define VMS_ILE_CUSTOM 1
#include "VConfig.h"

#endif


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

/******************
 *----  Init  ----*
 ******************/

#if defined(__VMS)
VkRunTimes::VkRunTimes (
    wall_tick_t iETime,
    proc_tick_t iUTime,
    unsigned long iBufIO,
    unsigned long iDirIO,
    unsigned long iFilCnt,
    unsigned long iPageFlts
) : m_iETime (iETime),
    m_iUTime (iUTime),
    m_iBufIO (iBufIO),
    m_iDirIO (iDirIO),
    m_iFilCnt (iFilCnt),
    m_iPageFlts (iPageFlts)
{
}

#elif defined(_WIN32)
VkRunTimes::VkRunTimes (
    wall_tick_t iETime, proc_tick_t iUTime, proc_tick_t iSTime
) : m_iETime (iETime), m_iUTime (iUTime), m_iSTime (iSTime) {
}

#else
VkRunTimes::VkRunTimes (
    wall_tick_t iETime, rusage *pUsage
) : m_iETime (iETime), m_iUsage (pUsage) {
}

#endif

/******************
 *----  Copy  ----*
 ******************/

VkRunTimes::VkRunTimes (VkRunTimes const &rOther)
    : m_iETime (rOther.m_iETime)
#if defined(__VMS)
    , m_iUTime (rOther.m_iUTime)
    , m_iBufIO (rOther.m_iBufIO)
    , m_iDirIO (rOther.m_iDirIO)
    , m_iFilCnt (rOther.m_iFilCnt)
    , m_iPageFlts (rOther.m_iPageFlts)
#elif defined(_WIN32)
    , m_iUTime (rOther.m_iUTime)
    , m_iSTime (rOther.m_iSTime)
#else
    , m_iUsage (rOther.m_iUsage)
#endif
{
}


/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

/************************
 *****  Assignment  *****
 ************************/

#if defined(__VMS)
VkRunTimes &VkRunTimes::operator= (VkRunTimes const &rOther) {
    m_iETime = rOther.m_iETime;
    m_iUTime = rOther.m_iUTime;
    m_iBufIO = rOther.m_iBufIO;
    m_iDirIO = rOther.m_iDirIO;
    m_iFilCnt = rOther.m_iFilCnt;
    m_iPageFlts = rOther.m_iPageFlts;

    return *this;
}

VkRunTimes &VkRunTimes::operator+= (VkRunTimes const &rOther) {
    m_iETime += rOther.m_iETime;
    m_iUTime += rOther.m_iUTime;
    m_iBufIO += rOther.m_iBufIO;
    m_iDirIO += rOther.m_iDirIO;
    m_iFilCnt += rOther.m_iFilCnt;
    m_iPageFlts += rOther.m_iPageFlts;

    return *this;
}

VkRunTimes &VkRunTimes::operator-= (VkRunTimes const &rOther) {
    m_iETime -= rOther.m_iETime;
    m_iUTime -= rOther.m_iUTime;
    m_iBufIO -= rOther.m_iBufIO;
    m_iDirIO -= rOther.m_iDirIO;
    m_iFilCnt -= rOther.m_iFilCnt;
    m_iPageFlts -= rOther.m_iPageFlts;

    return *this;
}

#elif defined(_WIN32)
VkRunTimes &VkRunTimes::operator= (VkRunTimes const &rOther) {
    m_iETime = rOther.m_iETime;
    m_iUTime = rOther.m_iUTime;
    m_iSTime = rOther.m_iSTime;

    return *this;
}

VkRunTimes &VkRunTimes::operator+= (VkRunTimes const &rOther) {
    m_iETime += rOther.m_iETime;
    m_iUTime += rOther.m_iUTime;
    m_iSTime += rOther.m_iSTime;

    return *this;
}

VkRunTimes &VkRunTimes::operator-= (VkRunTimes const &rOther) {
    m_iETime -= rOther.m_iETime;
    m_iUTime -= rOther.m_iUTime;
    m_iSTime -= rOther.m_iSTime;

    return *this;
}

#else
VkRunTimes &VkRunTimes::operator= (VkRunTimes const &rOther) {
    m_iETime = rOther.m_iETime;
    m_iUsage = rOther.m_iUsage;

    return *this;
}

VkRunTimes &VkRunTimes::operator+= (VkRunTimes const &rOther) {
    m_iETime += rOther.m_iETime;
    m_iUsage += rOther.m_iUsage;

    return *this;
}

VkRunTimes &VkRunTimes::operator-= (VkRunTimes const &rOther) {
    m_iETime -= rOther.m_iETime;
    m_iUsage -= rOther.m_iUsage;

    return *this;
}
#endif


/*************************
 *****  Measurement  *****
 *************************/

#if defined (_WIN32)

/************************
 *----  Windows/NT  ----*
 ************************/

double const VkRunTimes::g_iElapsedTicksPerSecond = 1e7;
double const VkRunTimes::g_iProcessorTicksPerSecond = 1e7;

#define FileTimeToTicks(ft) (*reinterpret_cast<tick_count_t*>(&(ft)))

void VkRunTimes::refresh () {
    SYSTEMTIME		currentSystemTime;
    FILETIME		currentTime;
    FILETIME		processCreationTime;
    FILETIME		processExitTime;
    FILETIME		processKernelTime;
    FILETIME		processUserTime;

    GetSystemTime	(&currentSystemTime);
    SystemTimeToFileTime(&currentSystemTime, &currentTime);

    GetProcessTimes (
	GetCurrentProcess (),
	&processCreationTime,
	&processExitTime,
	&processKernelTime,
	&processUserTime
    );

    m_iETime = FileTimeToTicks (currentTime) - FileTimeToTicks (processCreationTime);
    m_iUTime = FileTimeToTicks (processUserTime);
    m_iSTime = FileTimeToTicks (processKernelTime);
}


#elif defined (__VMS)

/*********************************
 *----          VMS          ----*
 *********************************/

#ifndef __NEW_STARLET
#define __NEW_STARLET 1
#endif
#include <starlet.h>
#include <gen64def.h>
#include <stsdef.h>
#include <jpidef.h>
#include <iledef.h>
#include <iosbdef.h>

double const VkRunTimes::g_iElapsedTicksPerSecond = 1e7;
double const VkRunTimes::g_iProcessorTicksPerSecond = 1e2;

#define COUNTOF(array) (sizeof (array)/sizeof (array)[0])

void VkRunTimes::refresh () {
    static wall_tick_t g_iElapsedBase = 0;

    GENERIC_64 tim;
    if (!$VMS_STATUS_SUCCESS (sys$gettim(&tim)))
	m_iETime = 0;
    else if (g_iElapsedBase)
	m_iETime = tim.gen64$q_quadword - g_iElapsedBase;
    else {
	g_iElapsedBase = tim.gen64$q_quadword;
	m_iETime = 0;
    }

    struct {
      unsigned long * pitem;
      short jpi_code;
    } const item_list[]={
	{&m_iUTime, JPI$_CPUTIM},
	{&m_iBufIO, JPI$_BUFIO},
	{&m_iDirIO, JPI$_DIRIO},
	{&m_iFilCnt, JPI$_FILCNT},
	{&m_iPageFlts, JPI$_PAGEFLTS}
    };
    ile_length_t len[COUNTOF(item_list)] = {0};
    // the extra +1 is used to terminate the list
    ile3_t ile_array[COUNTOF(item_list)+1] = {0}; 
    for(unsigned i=0;i < COUNTOF(item_list);i++) {
	ile3_t this_ile = ILE3_BUF (item_list[i].jpi_code, sizeof(*(item_list[i].pitem)), item_list[i].pitem, &len[i]);
	ile_array[i] = this_ile;
    }
      
    IOSB siosb;
    sys$getjpiw(0,NULL,NULL,ile_array,&siosb,NULL,NULL);
}

#else  // not VMS or Windows

/*********************************
 *----  HP-UX/Solaris/Linux  ----*
 *********************************/

double const VkRunTimes::g_iElapsedTicksPerSecond = 1e6;
double const VkRunTimes::g_iProcessorTicksPerSecond = 1e6;

void VkRunTimes::xtimeval_t::refresh () {
    gettimeofday (this, 0);

    static wall_tick_t g_iElapsedBase = 0;
    if (g_iElapsedBase)
	*this -= g_iElapsedBase;
    else {
	g_iElapsedBase = *this;
	*this = 0;
    }
}

void VkRunTimes::xrusage_t::refresh () {
    static bool bBaseUsageUnknown = true;
    static xrusage_t gBaseUsage (0);

    getrusage (RUSAGE_SELF, this);

    if (bBaseUsageUnknown) {
	bBaseUsageUnknown = false;
	gBaseUsage.setTo (this);
    }
    *this -= (gBaseUsage);
}

void VkRunTimes::xrusage_t::setTo (rusage const &rOther) {
    memcpy (static_cast<rusage*>(this), &rOther, sizeof (rusage));
}

void VkRunTimes::xrusage_t::setTo (rusage const *pOther) {
    if (pOther)
	setTo (*pOther);
    else
	memset (static_cast<rusage*>(this), 0, sizeof (rusage));
}

VkRunTimes::xrusage_t &VkRunTimes::xrusage_t::operator+= (rusage const &rOther) {
    xtimeval_t iThisTime (0), iAnotherTime (0);

    iThisTime.setTo (ru_utime);
    iAnotherTime.setTo (rOther.ru_utime);
    iThisTime += iAnotherTime;
    ru_utime = iThisTime;

    iThisTime.setTo (ru_stime);
    iAnotherTime.setTo (rOther.ru_stime);
    iThisTime += iAnotherTime;
    ru_stime = iThisTime;

    ru_maxrss += rOther.ru_maxrss;
    ru_ixrss += rOther.ru_ixrss;
    ru_idrss += rOther.ru_idrss;
    ru_isrss += rOther.ru_isrss;
    ru_minflt += rOther.ru_minflt;
    ru_majflt += rOther.ru_majflt;
    ru_nswap += rOther.ru_nswap;
    ru_inblock += rOther.ru_inblock;
    ru_oublock += rOther.ru_oublock;
    ru_msgsnd += rOther.ru_msgsnd;
    ru_msgrcv += rOther.ru_msgrcv;
    ru_nsignals += rOther.ru_nsignals;
    ru_nvcsw += rOther.ru_nvcsw;
    ru_nivcsw += rOther.ru_nivcsw;
    
    return *this;
}

VkRunTimes::xrusage_t &VkRunTimes::xrusage_t::operator-= (rusage const &rOther) {
    xtimeval_t iThisTime (0), iAnotherTime (0);

    iThisTime.setTo (ru_utime);
    iAnotherTime.setTo (rOther.ru_utime);
    iThisTime -= iAnotherTime;
    ru_utime = iThisTime;

    iThisTime.setTo (ru_stime);
    iAnotherTime.setTo (rOther.ru_stime);
    iThisTime -= iAnotherTime;
    ru_stime = iThisTime;

    ru_maxrss -= rOther.ru_maxrss;
    ru_ixrss -= rOther.ru_ixrss;
    ru_idrss -= rOther.ru_idrss;
    ru_isrss -= rOther.ru_isrss;
    ru_minflt -= rOther.ru_minflt;
    ru_majflt -= rOther.ru_majflt;
    ru_nswap -= rOther.ru_nswap;
    ru_inblock -= rOther.ru_inblock;
    ru_oublock -= rOther.ru_oublock;
    ru_msgsnd -= rOther.ru_msgsnd;
    ru_msgrcv -= rOther.ru_msgrcv;
    ru_nsignals -= rOther.ru_nsignals;
    ru_nvcsw -= rOther.ru_nvcsw;
    ru_nivcsw -= rOther.ru_nivcsw;
    
    return *this;
}

void VkRunTimes::refresh () {
    m_iETime.refresh ();
    m_iUsage.refresh ();
}

#endif
