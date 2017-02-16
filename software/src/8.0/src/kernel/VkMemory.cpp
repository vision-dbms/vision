/*****  Vision Kernel File Mapping Interface  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

#include "VpFileMapping.h"

typedef unsigned __int64 size64_t;

/******************
 *****  Self  *****
 ******************/

#include "VkMemory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VkStatus.h"

#if defined(_WIN32)
#pragma comment (lib, "advapi32.lib")

#elif defined(__VMS)

#ifndef __NEW_STARLET
#define __NEW_STARLET
#endif

#ifndef _LARGEFILE
#define _LARGEFILE
#endif

#define USING_VMS_SYSTEM_SERVICES

#include <efndef.h>
#include <gen64def.h>

#define VMS_ILE_CUSTOM 1
#define V_USING_CUSTOM_DESCRIPTOR 1
#include "VConfig.h"

#include <iosadef.h>
#include <iosbdef.h>
#include <rms.h>
#include <secdef.h>
#include <ssdef.h>
#include <starlet.h>
#include <stsdef.h>
#include <syidef.h>
#include <tis.h>
#include <vadef.h>

#include "V_VString.h"		// omit:Linux omit:SunOS
#include "Vos_VMSMemoryRegion.h"// omit:Linux omit:SunOS

#include "cast_native.h"	// omit:Linux omit:SunOS

#ifdef __INITIAL_POINTER_SIZE
#pragma __required_pointer_size __save
#pragma __required_pointer_size __long
#endif

typedef void *pointer64_t;
extern "C" {
    typedef void (*astptr_t)(__unknown_params);
}

#ifdef __INITIAL_POINTER_SIZE
#pragma __required_pointer_size __restore
#endif


extern "C" {
/***** >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< *****
 *****  The following declarations refer to the undocumented routines	*****
 *****  used by the C run time library to manage virtual address space	*****
 *****  for the process.  Calling these routines allows address space	*****
 *****  no longer needed for one purpose (e.g., mapping) to be reused	*****
 *****  for another (e.g., malloc). (Thanks to Eric Milkie for pointing	*****
 *****  us to the source listings for the C-RTL).			*****
 ***** >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< *****/

    extern int decc$$translate (int xVMSStatus);

/***** >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< *****
 *****    sChunk 	- number of 512 byte blocks to allocate.  For	*****
 *****			  crmpsc mapping purposes, must be a multiple	*****
 *****			  of the CPU page size.				*****
 *****    pChunk	- a two element array of pointers that point	*****
 *****			  to the beginning and end of the address	*****
 *****			  space.					*****
 ***** >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< *****/
    extern int decc$$mman_get_chunk64 (unsigned long sChunk, void *pChunk[2]);
    extern int decc$$mman_free_chunk64 (void *pChunk[2]);
#   define decc$$mman_get_chunk  decc$$mman_get_chunk64
#   define decc$$mman_free_chunk decc$$mman_free_chunk64
}


/*********************************************************
 *********************************************************
 *****  VMS <-> Unix File Name Conversion Utilities  *****
 *********************************************************
 *********************************************************/

namespace V {
    namespace OS {
    /**********************************
     *****  File Name Conversion  *****
     **********************************/
	bool ConvertUnixFileNameToVms (VString &rConvertedName, char const *pFileName);
	bool ConvertVmsFileNameToUnix (VString &rConvertedName, char const *pFileName);
    }
}


/************************************
 ************************************
 *****  VMS System Measurement  *****
 ************************************
 ************************************/

namespace V {
    namespace OS {
    /***********************************************************
     *****  Available Global Section Resource Measurement  *****
     ***********************************************************/

	bool MeasureAvailableGlobalSectionResource (unsigned int &rcFreeSections, unsigned int &rcFreePagelets) {
	    ile3_t iItemList[] = {
		ILE3_VAR (SYI$_FREE_GBLSECTS, rcFreeSections),
		ILE3_VAR (SYI$_FREE_GBLPAGES, rcFreePagelets),
		ILE3_END ()
	    };
	    _iosb iIOSB;
	    iIOSB.iosb$w_status = SS$_NORMAL;
	    return $VMS_STATUS_SUCCESS (sys$getsyi(EFN$C_ENF, 0, 0, iItemList, &iIOSB, (astptr_t)&tis_io_complete, 0))
		&& $VMS_STATUS_SUCCESS (tis_synch (EFN$C_ENF, &iIOSB))
		&& $VMS_STATUS_SUCCESS (iIOSB.iosb$w_status);
	}


    /************************
     *****  SystemInfo  *****
     ************************/
	class SystemInfo {
	public:
	    SystemInfo ();
	    ~SystemInfo () {
	    }
	    unsigned int maxGlobalPagelets () const {
		return m_cMaxGlobalPagelets;
	    }
	    unsigned int maxGlobalSections () const {
		return m_cMaxGlobalSections;
	    }
	    unsigned int pageSize () const {
		return m_sPage;
	    }
	public:
	    bool globalSectionUsageExceeded (size64_t sMapping, char const *pPathName) const;
	private:
	    unsigned int m_cMaxGlobalPagelets;
	    unsigned int m_cMaxGlobalSections;
	    unsigned int m_sPage;
	    unsigned int m_cMinFreeGlobalPagelets;
	    unsigned int m_cMinFreeGlobalSections;
	};
	SystemInfo g_iSystemInfo;
    }
}


/*********************************************
 *****  Byte <-> Page Count Conversions  *****
 *********************************************/

namespace {
    unsigned int const PageletSize = 512;
    unsigned int const PageletMask = PageletSize - 1;
    unsigned int const PageSize = V::OS::g_iSystemInfo.pageSize ();
    unsigned int const PageMask = PageSize - 1;

    unsigned int const PageletsPerPage = PageSize / PageletSize;

    bool Successful (unsigned int xStatus) {
	if ($VMS_STATUS_SUCCESS (xStatus))
	    return true;
	decc$$translate (xStatus);
	return false;
    }

    class VStatus : public VkStatus {
	DECLARE_FAMILY_MEMBERS (VStatus, VkStatus);

    //  Access
    public:
	VkMemory::MapStatus ifFailed (VkMemory::MapStatus xFailedStatus) const {
	    return isFailed () && !Successful (m_xCode) ? xFailedStatus : VkMemory::Success;
	}
	operator VkMemory::MapStatus () const {
	    return ifFailed (VkMemory::TryAgain);
	}
    };
}

namespace V {
    size64_t BytesToPagelets (size64_t cBytes) {
	return (cBytes + PageletMask) / PageletSize;
    }
    size64_t BytesToPageletBytes (size64_t cBytes) {
	return (cBytes + PageletMask) & ~PageletMask;
    }

    size64_t BytesToPages (size64_t cBytes) {
	return (cBytes + PageMask) / PageSize;
    }
    size64_t BytesToPageBytes (size64_t cBytes) {
	return (cBytes + PageMask) & ~PageMask;
    }
}


/**********************************************
 *****  V::OS::SystemInfo Implementation  *****
 **********************************************/

V::OS::SystemInfo::SystemInfo () {
    ile3_t iItemList[] = {
	ILE3_VAR (SYI$_GBLPAGES   , m_cMaxGlobalPagelets),
	ILE3_VAR (SYI$_GBLSECTIONS, m_cMaxGlobalSections),
	ILE3_VAR (SYI$_PAGE_SIZE  , m_sPage),
	ILE3_END ()
    };
    _iosb iIOSB;
    iIOSB.iosb$w_status = SS$_NORMAL;
    if ($VMS_STATUS_SUCCESS (sys$getsyi(EFN$C_ENF, 0, 0, iItemList, &iIOSB, (astptr_t)&tis_io_complete, 0))
    &&  $VMS_STATUS_SUCCESS (tis_synch (EFN$C_ENF, &iIOSB))
    &&  $VMS_STATUS_SUCCESS (iIOSB.iosb$w_status)) {
	m_cMinFreeGlobalPagelets = m_cMaxGlobalPagelets / 10;
	m_cMinFreeGlobalSections = m_cMaxGlobalSections / 10;
    } else {
	m_sPage = 16 * 512; // = 8k
	m_cMaxGlobalPagelets = m_cMaxGlobalSections = UINT_MAX;
	m_cMinFreeGlobalPagelets = m_cMinFreeGlobalSections = 0;
    }
}

bool V::OS::SystemInfo::globalSectionUsageExceeded (size64_t sMapping, char const *pPathName) const {
    unsigned int cFreeGlobalSections, cFreeGlobalPagelets;
    return MeasureAvailableGlobalSectionResource (cFreeGlobalSections, cFreeGlobalPagelets) && (
	m_cMinFreeGlobalSections > cFreeGlobalSections ||
	m_cMinFreeGlobalPagelets > cFreeGlobalPagelets - BytesToPages (sMapping) / PageletsPerPage
    );
}


/******************************************
 ******************************************
 *****  VMSdecc$$mman Memory Manager  *****
 ******************************************
 ******************************************/

namespace V {
    namespace OS {
	class VMSdecc$$mman : public VMM {
	    DECLARE_CONCRETE_RTT (VMSdecc$$mman, VMM);

	//  Construction
	public:
	    static ThisClass *Instance ();
	private:
	    VMSdecc$$mman () {
	    }

	//  Destruction
	private:
	    ~VMSdecc$$mman () {
	    }

	//  Access/Query
	private:
	    virtual /*override*/ unsigned int adjustedSectionFlags_(unsigned int iFlags, addr64_t pSpace) const;
	    virtual /*override*/ bool getRegionID_(VkStatus &rStatus, _generic_64 &rRegionID, addr64_t pSpace) const;

	//  Use
	private:
	    virtual /*override*/ bool acquireSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea, Area const &rSpace);
	    virtual /*override*/ bool provideSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea, size64_t sSpace);
	    virtual /*override*/ bool reclaimSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea);
	};


/***************************
 *****  Run Time Type  *****
 ***************************/

	DEFINE_CONCRETE_RTT (VMSdecc$$mman);

/**************************
 *****  Construction  *****
 **************************/

	VMSdecc$$mman *VMSdecc$$mman::Instance () {
	    static Reference pInstance (new ThisClass ());;
	    return pInstance;
	}

/********************
 *****  Access  *****
 ********************/

	unsigned int VMSdecc$$mman::adjustedSectionFlags_(unsigned int iFlags, addr64_t pSpace) const {
	    return iFlags;
	}

	bool VMSdecc$$mman::getRegionID_(VkStatus &rStatus, _generic_64 &rRegionID, addr64_t pSpace) const {
	    _regsum iRegionInfo; unsigned int sRegionInfo;
	    if (!rStatus.MakeSuccessStatus (
		    sys$get_region_info (
			VA$_REGSUM_BY_VA, 0, pSpace, 0, sizeof (iRegionInfo), &iRegionInfo, &sRegionInfo
		    )
		)
	    ) return false;

	    rRegionID.gen64$q_quadword = iRegionInfo.va$q_region_id;

	    return rStatus.MakeSuccessStatus ();
	}

/*****************
 *****  Use  *****
 *****************/

	bool VMSdecc$$mman::acquireSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea, Area const &rSpace) {
	    return rArea.contains (rSpace) ? rStatus.MakeSuccessStatus () :rStatus.MakeErrorStatus (SS$_PAGNOTINREG);
	}

	bool VMSdecc$$mman::provideSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea, size64_t sSpace) {
	    void *pChunk[2]; //  decc$$mman_get_chunk expects a 32-bit pointer, make sure it gets one in 64-bit land.
	    if (!rStatus.MakeSuccessStatus (
		    decc$$mman_get_chunk (static_cast<unsigned long>(BytesToPagelets(sSpace)), pChunk)
		)
	    ) return false;

	    rArea.setTo (
		reinterpret_cast<pointer_t>(pChunk[0]), reinterpret_cast<size64_t>(pChunk[1]) - reinterpret_cast<size64_t>(pChunk[0]) + 1
	    );
	    rpMM.setTo (this);

	    return rStatus.MakeSuccessStatus ();
	}

	bool VMSdecc$$mman::reclaimSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea) {
	  void *pChunk[2] = {
	    //I *think* Area stores the pointers supplied by acquireSpace as 64-bit
	    //pointers, even if compiled in 32-bit mode.  In 32-bit mode it is safe
	    //to truncate this value, in 64-bit mode, cast_native_truncate does nothing
	    cast_native_truncate(rArea.areaOrigin ()),
	    cast_native_truncate(rArea.areaExtent () - 1)
	  };
	    rpMM.clear ();
	    rArea.clear ();
	    return rStatus.MakeSuccessStatus (decc$$mman_free_chunk (pChunk));
	}
    }
}


/********************************
 ********************************
 *****  VMS Memory Manager  *****
 ********************************
 ********************************/

static V::VMM *MemoryManager () {
    static V::VMM::Pointer g_pManager;

    if (!g_pManager) {
//  We're doing memory mapping so increase this process' working set limit to its maximum...
	if (!getenv ("VNoWSLAdjust")) {
	    unsigned int sNewWSL;
	    sys$adjwsl (INT_MAX, &sNewWSL);
	}

//  And create an appropriate address space manager...
	V::VMM::Reference pNewManager;
	pNewManager.setTo (V::OS::VMSMemoryRegion::New (0x10000000000)); // == 1TB
	if (pNewManager.isNil ())
	    pNewManager.setTo (V::OS::VMSdecc$$mman::Instance ());
	if (g_pManager.interlockedSetIfNil (pNewManager))
	    g_pManager->retain ();	//  ...make sure the provider is never reclaimed.
    }
    return g_pManager;
}
#endif // defined(__VMS)


/*******************
 *******************
 *****  State  *****
 *******************
 *******************/

unsigned int VkMemory::g_cMappedFiles = 0;


/**********************
 **********************
 *****  Allocate  *****
 **********************
 **********************/

bool VkMemory::Allocate (size_t regionSize, bool shared) {
#if defined(_WIN32)
/*****  Build an inheritance descriptor, ...  *****/
    SECURITY_ATTRIBUTES sa;
    sa.nLength			= sizeof (SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor	= NilOf (LPVOID);
    sa.bInheritHandle		= shared;

/*****  ... create a mapping object, ...  *****/
    HANDLE hMapping = CreateFileMapping (
	(HANDLE)0xffffffff,
	&sa,
	PAGE_READWRITE,
	0,
	regionSize,
	NilOf (char *)
    );

    if (NULL == hMapping) {
	errno = EACCES;
	return false;
    }

/*****  ... and map it:  *****/
    void *regionAddress = MapViewOfFile (hMapping, FILE_MAP_WRITE, 0, 0, 0);
    if (NULL == regionAddress) {
	CloseHandle (hMapping);
	errno = EACCES;
	return false;
    }

    m_iHandle = hMapping;

#elif defined(USING_VMS_SYSTEM_SERVICES)
    void *regionAddress = 0;
    errno = ENOTSUP;
    return false;

#elif defined(__VMS) || defined(_AIX) || defined(__hp9000s800) || defined (__hp9000s700)
/*****  Allocate an anonymous region, ...  *****/
    void *regionAddress = (void *)mmap (
	0,
	regionSize,
	PROT_READ | PROT_WRITE,
	(shared ? MAP_SHARED : MAP_PRIVATE) | MAP_ANONYMOUS | MAP_VARIABLE,
	-1,
	(off_t)0
    );
    if (MAP_FAILED == regionAddress)
	return false;

#elif defined (sun)
/*****  Allocate an anonymous region, ...  *****/
    int fd = open ("/dev/zero", O_RDWR);
    if (fd < 0)
	return false;
    void *regionAddress = (void *)mmap (
	0,
	regionSize,
	PROT_READ | PROT_WRITE,
	shared ? MAP_SHARED : MAP_PRIVATE,
	fd,
	(off_t)0
    );
    close (fd);
    if (MAP_FAILED == regionAddress)
	return false;
#else
/*****  Allocate an anonymous region, ...  *****/
    if (shared)
	return false;

    void *regionAddress = allocate (regionSize);
    if (IsNil (regionAddress))
	return false;

    memset (regionAddress, 0, regionSize);
#endif

#ifndef USING_VMS_SYSTEM_SERVICES
    g_cMappedFiles++;

    m_pRegion = regionAddress;
    m_sRegion = regionSize;

    return true;
#endif
}


/**********************************************************
 **********************************************************
 *****  Global Reclamation Manager - HP9000s700 Only  *****
 **********************************************************
 **********************************************************/

#if defined(__hp9000s800) || defined(__hp9000s700)

/*****************************************************
 *****  Global Reclamation Manager Declarations  *****
 *****************************************************/

#define GRMMutexIndex		0
#define GRMReplyCountIndex	1
#define GRMRequestCountIndex	2
#define GRMProcessCountIndex	3

#define GRMSemaphoreCount (GRMProcessCountIndex + 1)
#define GRMInitialized    (GRMID >= 0)

PrivateFnDef void defaultErrorHandler (char const *pMessage) {
    printf (pMessage);
    exit (1);
}

PrivateVarDef int GRMID = -1;
PrivateVarDef int TracingGRMActivity = false;

PrivateVarDef VkMemoryTraceCallback TraceCallBack = (VkMemoryTraceCallback) printf;
PrivateVarDef VkMemoryErrorCallback ErrorCallBack = defaultErrorHandler;


/************************************************
 *****  Global Reclamation Manager Display  *****
 ************************************************/

PrivateFnDef void GRMDisplay () {
    int	i;

    TraceCallBack ("+++ GRM Semaphore Value: ");
    for (i = 0; i < GRMSemaphoreCount; i++) TraceCallBack (
	" %d(%d)", semctl (GRMID, i, GETVAL), semctl (GRMID, i, GETNCNT)
    );
    TraceCallBack ("\n");
}


/***************************************************
 *****  Global Reclamation Manager Adjustment  *****
 ***************************************************/

PrivateFnDef int GRMAdjust (int semNumber, int semAdjustment, int semFlags) {
    struct sembuf semop1[1];

    semop1[0].sem_num  = semNumber;
    semop1[0].sem_op   = semAdjustment;
    semop1[0].sem_flg  = semFlags;

    return semop (GRMID, semop1, 1);
}


/****************************************************************
 *****  Global Reclamation Manager Acquisition and Release  *****
 ****************************************************************/

PrivateFnDef void GRMAcquire (int semNumber, int semAdjustment, int semFlags) {
    struct sembuf semop1[2];

    semop1[0].sem_num	= GRMMutexIndex;
    semop1[0].sem_op	= -1;
    semop1[0].sem_flg	= SEM_UNDO;
    semop1[1].sem_num	= semNumber;
    semop1[1].sem_op	= semAdjustment;
    semop1[1].sem_flg	= semFlags;

    if (semop (GRMID, semop1, 2) < 0 && errno != EAGAIN) ErrorCallBack (
	"Global Reclamation Manager Lock Acquistion"
    );
}

PrivateFnDef void GRMRelease () {
    struct sembuf semop1[1];

    semop1[0].sem_num	= GRMMutexIndex;
    semop1[0].sem_op	= 1;
    semop1[0].sem_flg	= SEM_UNDO;

    if (semop (GRMID, semop1, 1) < 0) ErrorCallBack (
	"Global Reclamation Manager Lock Release"
    );
}


/**********************************************
 *****  Global Reclamation Manager Query  *****
 **********************************************/

PrivateFnDef int GRMReplyCount () {
    int count = semctl (GRMID, GRMReplyCountIndex, GETVAL);
    if (count < 0) ErrorCallBack (
	"Global Reclamation Manager Reply Count Access"
    );
    return count;
}

PrivateFnDef int GRMRequestCount () {
    int count = semctl (GRMID, GRMRequestCountIndex, GETVAL);
    if (count < 0) ErrorCallBack (
	"Global Reclamation Manager Request Count Access"
    );
    return count;
}

PublicFnDef int VkMemory_GRMRequestCount () {
    return GRMInitialized ? GRMRequestCount () : 0;
}

PrivateFnDef int GRMProcessCount () {
    int count = semctl (GRMID, GRMProcessCountIndex, GETVAL);
    if (count < 0) ErrorCallBack (
	"Global Reclamation Manager Process Count Access"
    );
    return count;
}

PrivateFnDef int GRMPotentialServerCount () {
    return GRMProcessCount () - GRMRequestCount () - 1;
}


/**********************************************************
 *****  Global Reclamation Manager Reclamation Reply  *****
 **********************************************************/

PrivateFnDef void GRMReportRecovery () {
    int reply = GRMRequestCount () - GRMReplyCount ();

    if (reply <= 0)
	return;

    if (TracingGRMActivity) {
	TraceCallBack (
	    "+++ Reply To %d Request%s Generated\n", reply, reply > 1 ? "s": ""
	);
	GRMDisplay ();
    }
    if (GRMAdjust (GRMReplyCountIndex, reply, 0) < 0) ErrorCallBack (
	"Global Reclamation Manager Report Generation"
    );
}


/************************************************************
 *****  Global Reclamation Manager Reclamation Request  *****
 ************************************************************/

PrivateFnDef void GRMRequestRecovery () {
    if (GRMPotentialServerCount () > 0) {
	GRMAdjust	  (GRMRequestCountIndex, 1, SEM_UNDO);
	GRMAcquire	  (GRMReplyCountIndex, -1, 0);
	GRMAdjust	  (GRMRequestCountIndex, -1, SEM_UNDO | IPC_NOWAIT);
	GRMRelease	  ();
	GRMReportRecovery ();
	if (TracingGRMActivity) {
	    TraceCallBack (	"+++ Reply Received By Process %d\n", getpid ());
	    GRMDisplay ();
	}
    }
}
#endif


/*******************************************************
 *****  Global Reclamation Manager Initialization  *****
 *******************************************************/

void VkMemory::StartGRM (
    char const			*callerName,
    char const			*logFacility,
    VkMemoryTraceCallback	traceFn,
    VkMemoryErrorCallback	errorFn
) {
#if defined(__hp9000s800) || defined(__hp9000s700)
    key_t			semkey;
    static bool			initializationAttempted = false;
    int				facility = LOG_USER;

/*****  Check for multiple initialization calls...  *****/
    if (initializationAttempted)
	return;

    initializationAttempted = true;
    char const* estring = logFacility;
    if (IsNil (estring))
	estring = getenv ("VisionGRMlogFacility");
    if (IsntNil (estring)) {
	if (0 == strcmp (estring, "local0")) facility = LOG_LOCAL0;
	else if (0 == strcmp (estring, "local1")) facility = LOG_LOCAL1;
	else if (0 == strcmp (estring, "local2")) facility = LOG_LOCAL2;
	else if (0 == strcmp (estring, "local3")) facility = LOG_LOCAL3;
	else if (0 == strcmp (estring, "local4")) facility = LOG_LOCAL4;
	else if (0 == strcmp (estring, "local5")) facility = LOG_LOCAL5;
	else if (0 == strcmp (estring, "local6")) facility = LOG_LOCAL6;
	else if (0 == strcmp (estring, "local7")) facility = LOG_LOCAL7;
	else if (0 == strcmp (estring, "daemon")) facility = LOG_DAEMON;
    }
/*  printf ("\"%s\" (%d)\n", estring, facility);  */
    if (IsntNil (callerName)) openlog (callerName, LOG_PID, facility);
    else 		      openlog ("VkMemory", LOG_PID, facility);
    if (IsntNil (estring = getenv ("VisionGRMEnabled")) && 0 == atoi (estring)) {
	syslog (LOG_INFO, "Not participating in GRM");
	return;
    }

    if (IsntNil (estring = getenv ("VisionGRMTrace")))
	TracingGRMActivity = atoi (estring) ? true : false;


    if (IsntNil (traceFn))
	TraceCallBack = traceFn;
    if (IsntNil (errorFn))
	ErrorCallBack = errorFn;
    
/*****  Create or access the semaphore...  *****/
    semkey = ftok ("/", 'V');
    GRMID = semget (semkey, GRMSemaphoreCount, IPC_CREAT | IPC_EXCL | 0666);
    if (GRMInitialized) {
	if (TracingGRMActivity) TraceCallBack ("+++ GRM Semaphore Created\n");
	if (semctl (GRMID, 0, SETVAL, 1) < 0) ErrorCallBack (
	    "Global Reclamation Manager Semaphore Initialization"
	);
    }
    else if ((GRMID = semget (semkey, GRMSemaphoreCount, 0666)) < 0
    ) ErrorCallBack ("Global Reclamation Manager Semaphore Access");

/*****  ... and attach this process to the semaphore ...  *****/
    GRMAcquire (GRMProcessCountIndex, 1, SEM_UNDO);

/*****  ... arrange for reply to be generated at exit ...  *****/
    while (GRMAdjust (GRMReplyCountIndex, -1, SEM_UNDO | IPC_NOWAIT) < 0) {
	if (errno != EAGAIN) ErrorCallBack (
	    "Global Reclamation Manager Initial Reply Count Decrement"
	);
	if (GRMAdjust (GRMReplyCountIndex, 1, 0) < 0) ErrorCallBack (
	    "Global Reclamation Manager Initial Reply Count Increment"
	);
    }

    GRMRelease ();
    if (TracingGRMActivity)
	GRMDisplay ();
#endif
}

/*************************************************
 *****  Global Reclamation Manager Shutdown  *****
 *************************************************/

void VkMemory::StopGRM () {
#if defined(__hp9000s800) || defined(__hp9000s700)
    /** needed only if the process is doing an exec rather than an exit **/
    GRMAcquire (GRMProcessCountIndex, -1, SEM_UNDO);
    GRMAdjust  (GRMReplyCountIndex, 1, SEM_UNDO);
    GRMRelease ();
#endif
}


/*******************************
 *******************************
 *****  MapImplementation  *****
 *******************************
 *******************************/

VkMemory::MapStatus VkMemory::Map (
    char const *pPathName, bool fReadOnly, size_t sMultiplier, size_t sIncrement, Share xShare
) {
#if defined(_WIN32)
    static int			fSDInitialized = false;
    static char			iSD[SECURITY_DESCRIPTOR_MIN_LENGTH];
    int 			lastError;

    PSECURITY_DESCRIPTOR	pSD = (PSECURITY_DESCRIPTOR)&iSD;
    SECURITY_ATTRIBUTES		iSA;

    char			fnameBuffer[MAX_PATH];
    char			mnameBuffer[MAX_PATH];
    char *			nameCursor;
    HANDLE			hFile, hMapping;

/*****  Build a security descriptor for any mapping object created, ...  *****/
    if (fSDInitialized);
    else if (
	InitializeSecurityDescriptor (pSD, SECURITY_DESCRIPTOR_REVISION)
	&& SetSecurityDescriptorDacl (pSD, true, (PACL)NULL, false)
    ) fSDInitialized = true;
    else
    {
	errno = EINVAL;
	return Failed;
    }

    iSA.nLength			= sizeof (SECURITY_ATTRIBUTES);
    iSA.lpSecurityDescriptor	= pSD;
    iSA.bInheritHandle		= false;

/*****  Convert the file name to an absolute DOS path...  *****/
    strcpy (mnameBuffer, pPathName);
    for (nameCursor = strchr (mnameBuffer, '/'); IsntNil (nameCursor); nameCursor = strchr (mnameBuffer, '/'))
	*nameCursor = '\\';
    if (0 == GetFullPathName (mnameBuffer, sizeof (fnameBuffer), fnameBuffer, &nameCursor)) {
	errno = ENOENT;
	return Failed;
    }

/*****  ... generate a mapping name, ...  *****/
    strcpy (mnameBuffer, fnameBuffer);
    for (nameCursor = strchr (mnameBuffer, '\\'); IsntNil (nameCursor); nameCursor = strchr (mnameBuffer, '\\'))
	*nameCursor = '/';


/*****  If the mapping object cannot be opened, ...  *****/
    /*****  ... open the file, ...  *****/
    hFile = CreateFile (
	fnameBuffer,
	fReadOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE,
	fReadOnly ? FILE_SHARE_READ : 0,
	NULL,
	OPEN_EXISTING,
	FILE_ATTRIBUTE_NORMAL,
	NULL
	);
    if (FileHandleIsInvalid (hFile)) {
	errno = ENOENT;
	return Failed;
    }

    /*****  ... obtain its size, ...  *****/
    size_t regionSize = GetFileSize (hFile, NULL);
    if (0xffffffff == regionSize) {
	CloseHandle (hFile);
	errno = EACCES;
	return Failed;
    }
    regionSize = fReadOnly ? regionSize : regionSize * sMultiplier + sIncrement;

    /*****  ... and attempt to create it:  *****/
    hMapping = CreateFileMapping (
	    hFile,
	    &iSA,
	    fReadOnly ? PAGE_READONLY : PAGE_READWRITE,
	    0,
	    fReadOnly ? 0 : regionSize,
	    mnameBuffer
	);
    lastError = GetLastError();
    CloseHandle (hFile);

    if (SystemHandleIsInvalid (hMapping)) {
	errno = EACCES;
	return Failed;
    }

/*****  Finally, map a view of the file...  *****/
    void *regionAddress = MapViewOfFile (
	hMapping, fReadOnly ? FILE_MAP_READ : FILE_MAP_WRITE, 0, 0, 0
    );
    if (NULL == regionAddress) {
	CloseHandle (hMapping);
	errno = EACCES;
	return TryAgain;
    }

    m_iHandle = hMapping;


#elif defined(USING_VMS_SYSTEM_SERVICES)
/*****  Obtain the file's device name and file id, ...  *****/
    struct stat iStats;
    if (stat (pPathName, &iStats) < 0)
	return TryAgain;

/*****  ...  generate a name for the global section, ...  *****/
    char iGSName[64];

#define USING_CRTL_FORMAT
#ifdef USING_CRTL_FORMAT
/*****  ...  matching the initial name used by the CRTL, ...  *****/
/*****       (See DISK$VMS_DOC:[VMSSRC.V732.ACRTL.LIS]OFF64_MMAP.LIS, line 47338)    *****/
    V::OS::dsc_descriptor_s iGSNameDescriptor (
	iGSName, sprintf (
	    iGSName, "C$00000000%s%04X%04X%04X", iStats.st_dev, iStats.st_ino[0], iStats.st_ino[1], iStats.st_ino[2]
	)
    );
/***** >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< *****
 *****  The CRTL writes	a eight digit sequence number at iGSName[2...9]	*****
 *****  using 'sprintf'.  That has the effect of changing the first	*****
 *****  character of the device name to '\0'.  We have to do the same	*****
 *****  if we are going to match its section name. Note that we have	*****
 *****  to do that in a way that avoids calling 'strlen' on the name	*****
 *****  after the change.						*****
 ***** >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< *****/
    iGSName[10] = '\0';

#else // !USING_CRTL_FORMAT
/*****  ...  using our own name format, ...  *****/
    VMS_ILE_CUSTOM_FORMAT;
#endif

/*****  ... obtain the required chunk of page aligned virtual address space, ...  *****/
    VStatus iStatus; AddressSpace iSpace; _generic_64 hRegion;
    if (!iSpace.acquireSpace (
	    iStatus, MemoryManager (), V::BytesToPageBytes (iStats.st_size)
	) || !iSpace.getRegionID (iStatus, hRegion)
    ) return iStatus;

/*****  ... determine the mapping flags, ...  *****/
    unsigned int const iFlags = iSpace.adjustedSectionFlags (fReadOnly ? 0 : SEC$M_WRT);

/***** >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< *****
 *****  sys$mgblsc_64 and sys$crmpsc_gfile_64 expect to return 64-bit	*****
 *****  pointers.  C++ for Itanium doesn't currently understand 64-bit	*****
 *****  pointers (how many decades has this 64-bit architecture been	*****
 *****  touted as the future?), so the following declaration ensures	*****
 *****  that the system service gets 64-bits worth of memory in which	*****
 *****  to return its result.						*****
 ***** >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< *****/
    union {
	unsigned __int64 asStorage;	// ... make sure this is really 64-bits wide!!!
	pointer64_t	 asPointer64;
	void*		 asPointer;
    } pMapping;

/*****  ...  try to map the section, ...  *****/
    iStatus.MakeErrorStatus (SS$_NOSUCHSEC);

    Share xWhatsNext = xShare;
    bool bDone = false;

    do {
	unsigned int iTheseFlags = iFlags;
	Share xThisShare = Share_None;

	if (xWhatsNext & Share_System) {
	    iTheseFlags |= SEC$M_SYSGBL;
	    xThisShare = Share_System;
	}
	else if (xWhatsNext & Share_Group) {
	    xThisShare = Share_Group;
	}
	xWhatsNext = static_cast<Share>(xWhatsNext - xThisShare);

	switch (xThisShare) {
	case Share_System:
	case Share_Group:
	    bDone = iStatus.MakeSuccessStatus (
		sys$mgblsc_64 (
		    &iGSNameDescriptor,		// global section name
		    0,				// version identity (0 -> match any)
		    &hRegion,			// region id
		    0,				// section offset (0 -> beginning)
		    0,				// section length (0 -> all)
		    0,				// access mode (0 -> caller's access mode)
		    iTheseFlags,		// flags
		    &pMapping.asPointer64,	// mapped origin return
		    &m_sMapping,		// mapped length return
		    iSpace.areaOrigin ()	// mapping address
		)
	    );
	    break;
	default:
	    bDone = true;
	    break;
	}
    } while (!bDone);
    if (SS$_NOSUCHSEC == iStatus.Code ()) {
/*****  ... and, if the section didn't exist, open a channel to the file, ...  *****/
#pragma __pointer_size __save
#pragma __pointer_size __short

//  Create a 'NAM' block containing the device and file id obtained from 'stat' ...
	NAM iNAM;
	iNAM = cc$rms_nam;

	iNAM.nam$t_dvi[0] = static_cast<char>(strlen (iStats.st_dev));
	strncpy (iNAM.nam$t_dvi + 1, iStats.st_dev, sizeof (iNAM.nam$t_dvi) - 1);
	memcpy (iNAM.nam$w_fid, iStats.st_ino, sizeof (iNAM.nam$w_fid));

	FAB iFAB;
	iFAB = cc$rms_fab;

//  Open the file using the 'NAM' block...
	iFAB.fab$l_nam = static_cast<NAM*>(&iNAM);
	iFAB.fab$v_nam = true;

	iFAB.fab$v_bio = true;
	iFAB.fab$v_get = true;
	iFAB.fab$v_put = !fReadOnly;

	iFAB.fab$b_shr = FAB$M_SHRGET | FAB$M_SHRPUT | FAB$M_SHRUPD | FAB$M_SHRDEL | FAB$M_UPI;
	iFAB.fab$v_ufo = true;

	iFAB.fab$b_rtv = -1;

	if (!Successful (sys$open (static_cast<FAB*>(&iFAB))))
	    return TryAgain;
#pragma __pointer_size __restore

	unsigned short xChannel = static_cast<unsigned short>(iFAB.fab$l_stv);

/*****  ... and try to create it using the requested sharing modes:  *****/
	iStatus.MakeErrorStatus (SS$_BADPARAM);

	xWhatsNext = xShare;
	bDone = false;

	bool bLimitingGlobalSectionResources = true;
	do {
	    unsigned int iTheseFlags = iFlags;
	    Share xThisShare = Share_None;

	    if (xWhatsNext & Share_System) {
		iTheseFlags |= SEC$M_SYSGBL;
		xThisShare = Share_System;
	    }
	    else if (xWhatsNext & Share_Group) {
		xThisShare = Share_Group;
	    }
	    else if (xWhatsNext & Share_Private) {
		xThisShare = Share_Private;
	    }
	    xWhatsNext = static_cast<Share>(xWhatsNext - xThisShare);

	    switch (xThisShare) {
	    case Share_System:
	    case Share_Group:
		if (bLimitingGlobalSectionResources) {
		    bLimitingGlobalSectionResources = false;
		    if (V::OS::g_iSystemInfo.globalSectionUsageExceeded (iStats.st_size, pPathName))
			mappingsReclaimed ();
		}
		bDone = iStatus.MakeSuccessStatus (
		    sys$crmpsc_gfile_64 (
			&iGSNameDescriptor,	// global section name
			0,			// version identity (0 -> match any)
			0,			// file offset (0 -> beginning)
			0,			// file length (0 -> all)
			xChannel,		// channel
			&hRegion,		// region id
			0,			// section offset (0 -> beginning)
			0,			// access mode (0 -> caller's access mode)
			iTheseFlags,		// flags
			&pMapping.asPointer64,	// mapped origin return
			&m_sMapping,		// mapped length return
			0,			// fault cluster
			iSpace.areaOrigin (),	// mapping address
			0			// mapping extent  (0 -> all)
		    )
		);
		break;

	    case Share_Private:
		bDone = iStatus.MakeSuccessStatus (
		    sys$crmpsc_file_64 (
			&hRegion,		// region id
			0,			// file offset (0 -> beginning)
			0,			// file length (0 -> all)
			xChannel,		// channel
			0,			// access mode (0 -> caller's access mode)
			iTheseFlags,		// flags
			&pMapping.asPointer64,	// mapped origin return
			&m_sMapping,		// mapped length return
			0,			// fault cluster
			iSpace.areaOrigin ()	// mapping address
		    )
		);
		if (bDone) {
		    m_xChannel = xChannel;
		    xChannel = 0;
		}
		break;

	    case Share_None:
		bDone = true;
		break;
	    }
	} while (!bDone);

/***** >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< *****
 *****  The channel used to create a global section is only used to	*****
 *****	identify the file to VMS.  Once the global section has been	*****
 *****  created, the channel should be deassigned immediately.  If a	*****
 *****  private section is created, however, the channel is used for	*****
 *****  paging operations and must be kept open until the section is	*****
 *****  destroyed (OpenVMS Programming Concepts, Section 12.3.7.14).	*****
 ***** >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< *****/
	if (xChannel > 0) {
	    sys$dassgn (xChannel);
	    xChannel = 0;
	}
    }

    if (iStatus.isFailed () || !iSpace.acquireSpace (
	    iStatus, reinterpret_cast<pointer_t>(pMapping.asPointer), V::BytesToPageBytes (m_sMapping)
	) || !m_iSpace.acquireSpace (iStatus, iSpace)
    ) return iStatus;

/***** >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< *****
 *****  Two sizes must be maintained on VMS.  One, 'iStats.st_size' is	*****
 *****  the size of the file's actual content and is the size expected	*****
 *****  by the users of this class.  The mapping length returned by	*****
 *****  sys$crmpsc_gfile_64 in m_sMapping is the number of bytes in	*****
 *****  the global section actually backed by blocks on disk.  It is	*****
 *****  >= than the content size and a multiple of the device's disk	*****
 *****  block size.  It must be retained for use by deltva_64 when and	*****
 *****  if the mapping is deleted.					*****
 ***** >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< *****/
    void *regionAddress = pMapping.asPointer;
    size_t regionSize = static_cast<size_t>(iStats.st_size);


#else

/*****  Open the file, ...  *****/
    int fd = open (pPathName, fReadOnly ? O_RDONLY : O_RDWR);
    if (fd < 0)
	return Failed;

/*****  ... determine the file's size, ...  *****/
    struct stat fileStats;
    if (fstat (fd, &fileStats) < 0) {
	close (fd);
	return Failed;
    }

    size_t regionSize = fReadOnly 
	? (size_t)fileStats.st_size
	: (size_t)fileStats.st_size * sMultiplier + sIncrement;
    if (fileStats.st_size != regionSize && ftruncate (fd, regionSize) < 0) {
	close (fd);
	return Failed;
    }

/*****  ... map it, ...  *****/
#if defined(__VMS) || defined(_AIX) || defined(__hp9000s800) || defined(__hp9000s700)
    void *regionAddress = (void *)mmap (
	0,
 	regionSize,
	fReadOnly ? PROT_READ : PROT_READ | PROT_WRITE,
	MAP_SHARED | MAP_FILE | MAP_VARIABLE,
	fd,
	(off_t)0
    );
    if (MAP_FAILED == regionAddress)
	regionAddress = NilOf (void *);
#elif defined (sun) || defined (__linux__) || defined(__APPLE__)
    void *regionAddress = (void *)mmap (
	0,
	regionSize,
	fReadOnly ? PROT_READ : PROT_READ | PROT_WRITE,
	MAP_SHARED,
	fd,
	(off_t)0
    );
    if (MAP_FAILED == regionAddress)
	regionAddress = NilOf (void *);
#else
    if (!fReadOnly) {
	close (fd);
	errno = EINVAL;
	return Failed;
    }

    void *regionAddress = allocate (regionSize);
    if (IsntNil (regionAddress)) {
	unsigned int sizeRemaining, sizeRead;
	pointer_t segmentCursor;

	for (
	    sizeRemaining = regionSize, segmentCursor = (pointer_t)regionAddress;
	    sizeRemaining > 0;
	    sizeRemaining -= sizeRead, segmentCursor += sizeRead
	)
	{
	    sizeRead = (unsigned int)read (fd, (void *)segmentCursor, sizeRemaining);
	    if ((unsigned int)-1 == sizeRead || 0 == sizeRead)
	    {
		deallocate (regionAddress);
		regionAddress = NilOf (void *);
		errno = EIO;
		break;
	    }
	} 
    }
#endif
    close (fd);
    if (IsNil (regionAddress))
	return TryAgain;

#endif /* defined(_WIN32) */

    g_cMappedFiles++;

    m_pRegion = regionAddress;
    m_sRegion = regionSize;

    return Success;
}


/*****************
 *****************
 *****  Map  *****
 *****************
 *****************/

bool VkMemory::Map (char const *pPathName, bool fReadOnly, Share xShare) {
    MapStatus result = Map (pPathName, fReadOnly, 1, 0, xShare);
    if (TryAgain == result && mappingsReclaimed ()) {
	result = Map (pPathName, fReadOnly, 1, 0, xShare);
    }

#if defined(__hp9000s800) || defined(__hp9000s700)
    while (TryAgain == result && GRMInitialized && GRMPotentialServerCount () > 0) {
	if (TracingGRMActivity) {
	    TraceCallBack (	"+++ GRM wait for %s\n", pPathName);
	}

	GRMRequestRecovery ();
	result = Map (pPathName, fReadOnly, 1, 0, xShare);
    }
    if (Success != result && GRMInitialized) {
	int savedError = errno;
	syslog (LOG_WARNING, "GRM (%m) --> %s", pPathName);
	errno = savedError;
    }
#endif

    return result == Success ? true : false;
}


/*******************
 *******************
 *****  MapEx  *****
 *******************
 *******************/

bool VkMemory::Map (char const *pPathName, size_t sMultiplier, size_t sIncrement, Share xShare) {
    MapStatus result = Map (pPathName, false, sMultiplier, sIncrement, xShare);
    if (TryAgain == result && mappingsReclaimed ()) {
	result = Map (pPathName, false, sMultiplier, sIncrement, xShare);
    }

#if defined(__hp9000s800) || defined(__hp9000s700)
    while (TryAgain == result && GRMInitialized && GRMPotentialServerCount () > 0) {
	if (TracingGRMActivity) {
	    TraceCallBack (	"+++ GRM wait for %s\n", pPathName);
	}

	GRMRequestRecovery ();
	result = Map (pPathName, false, sMultiplier, sIncrement, xShare);
    }
    if (Success != result && GRMInitialized) {
	int savedError = errno;
	syslog (LOG_WARNING, "GRM (%m) --> %s", pPathName);
	errno = savedError;
    }
#endif
    return result == Success ? true : false;
}


/********************
 ********************
 *****  MapNew  *****
 ********************
 ********************/

bool VkMemory::MapNew (char const *pPathName, size_t sObject, Share xShare) {
    remove (pPathName);
    int fd = creat (pPathName, 0666);
    if (fd < 0)
	return false;
    close (fd);

    return Map (pPathName, 0, sObject, xShare);
}


/******************
 ******************
 *****  Sync  *****
 ******************
 ******************/

bool VkMemory::Refresh () {
#if defined(_WIN32) || defined(USING_VMS_SYSTEM_SERVICES)
    return true;
#else
    return msync (m_pRegion, m_sRegion, MS_INVALIDATE) < 0 ? false : true;
#endif
}

bool VkMemory::Sync () {
#if defined(_WIN32)
    return FlushViewOfFile (m_pRegion, m_sRegion) ? true : false;
#elif defined(USING_VMS_SYSTEM_SERVICES)
    _iosa iIOSA; pointer64_t pWritten; size64_t sWritten;
    return Successful (
	sys$updsec_64 (
	    m_pRegion, m_sRegion, 0, UPDFLG$M_WRT_MODIFIED, EFN$C_ENF, &iIOSA, &pWritten, &sWritten, (astptr_t)tis_io_complete, 0
 	)
    ) && Successful (tis_synch (EFN$C_ENF, &iIOSA)) && Successful (iIOSA.iosa$w_iosb_status);
#else
    return msync (m_pRegion, m_sRegion, MS_SYNC) < 0 ? false : true;
#endif
}


/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

void VkMemory::Destroy () {
    if (m_pRegion) {
#if defined(_WIN32)
	UnmapViewOfFile (m_pRegion);
	CloseHandle (m_iHandle);
#elif defined(USING_VMS_SYSTEM_SERVICES)
	VkStatus iStatus; _generic_64 hRegion; void *pDeleted; size64_t sDeleted;

	if (m_iSpace.getRegionID (iStatus, hRegion) && iStatus.MakeSuccessStatus (
/***** >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< *****
 *****  The CRTL implementation of munmap calls cretva... to replace	*****
 *****  the mapping instead of deltva... to delete it.  The rationale	*****
 *****  is to make sure that the library holds onto the memory.  We	*****
 *****  should do the same thing, BUT, the CRTL calls cretva twice -	*****
 *****  once in munmap and again in ...free_chunk.  Since we call	*****
 *****  free chunk, the CRTL should stay happy.  If, for some reason	*****
 *****  it doesn't this may be the place to look.  In any event, once	*****
 *****  we start relying on undocumented interfaces and behavior, why	*****
 *****  stop :-)							*****
 ***** >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>><<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< *****/
		sys$deltva_64 (&hRegion, m_pRegion, V::BytesToPageBytes (m_sMapping), 0, &pDeleted, &sDeleted)
	    ) && m_iSpace.releaseSpace (iStatus)
	) {
	    m_sMapping = 0;
	    if (m_xChannel > 0) {
		sys$dassgn (m_xChannel);
		m_xChannel = 0;
	    }
	}

#elif defined(__VMS) || defined(_AIX) || defined(__hp9000s800) || defined(__hp9000s700) || defined(sun) || defined(__linux__) || defined(__APPLE__)
	munmap (m_pRegion, m_sRegion);
#else
	deallocate (m_pRegion);
#endif

#if defined(__hp9000s800) || defined(__hp9000s700)
	if (GRMInitialized)
	    GRMReportRecovery ();
#endif
	m_pRegion = NilOf (void *);
	m_sRegion = 0;

	g_cMappedFiles--;
    }
}
