/*****  VTransientServicesForBatchvision Implementation  *****/

#define FacilityName vtRANSIENTsERVICESfORbATCHVISION

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

#include "VkStatus.h"

#if defined(_WIN32)
#include <malloc.h>
#endif

#if defined (__linux__) || defined (sun) || defined(__APPLE__)
#define USING_SYS_RESOURCE_LIMITS
#include <sys/resource.h>
#endif

/******************
 *****  Self  *****
 ******************/

#include "VTransientServicesForBatchvision.h"

/************************
 *****  Supporting  *****
 ************************/

#include "m.h"

#include "venvir.h"
#include "verr.h"
#include "vfac.h"
#include "vstdio.h"
#include "vutil.h"

#include "V_VAllocator.h"
#include "VOutputBuffer.h"

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VTransientServicesForBatchvision::VTransientServicesForBatchvision ()
    : m_sTotalMemoryAllocation 			(0)
    , m_sTotalMemoryAllocationHighWaterMark	(0)
    , m_sMemoryAllocationLimit			(0)
    , m_iErrorCount				(0)
    , m_sInitialGCThreshold			(static_cast<V::U64>(4096) * 1024 * 1024 * 1024)
    , m_sGCThreshold				(static_cast<V::U64>(4096) * 1024 * 1024 * 1024)
    , m_sGCSpread				(256 * 1024 * 1024)
{
#if defined (USING_SYS_RESOURCE_LIMITS)

    // Unless directed otherwise, ...
    // Don't allow unlimited datasize. If unlimited, fall back to 2 GB ..
    // Any finite limit is acceptable.
    struct rlimit datasize;
    if (-1 == getrlimit (RLIMIT_DATA, &datasize) ||  RLIM_INFINITY == datasize.rlim_cur){
	m_sMemoryAllocationLimit = getenv ("VisionDataLimitDisabled") ? 0xffffffff00000000ull :  0x80000000;
    } else {
	m_sMemoryAllocationLimit = datasize.rlim_cur;
    }
    datasize.rlim_cur = RLIM_INFINITY;
    setrlimit (RLIMIT_DATA, &datasize);
    enableStackDumps (true);
#endif
    char	 *pEnvVarValue;
    unsigned int iValue;
    if (IsntNil (pEnvVarValue = getenv ("VisionLogPath"))) {
	updateUsageLogSwitch (true);
	updateUsageLogFilePath (pEnvVarValue);
    }
    if (IsntNil (pEnvVarValue = getenv ("VisionLogTag"))) {
	updateUsageLogTag (pEnvVarValue);
    }
    if (IsntNil (pEnvVarValue = getenv ("VisionLogDelimiter"))) {
	updateUsageLogDelimiter (pEnvVarValue);
    }
    if (IsntNil (pEnvVarValue = getenv ("VisionLogSize")) &&
	(iValue = (unsigned int)strtoul (pEnvVarValue, (char **)NULL, 0)) > 0) {
	updateUsageLogSize (iValue);
    }
    if (IsntNil (pEnvVarValue = getenv ("VisionLogBackups"))) {
	iValue = (unsigned int)strtoul (pEnvVarValue, (char **)NULL, 0);
        updateUsageLogBackups (iValue);
    }

    if (IsntNil (pEnvVarValue = getenv ("VisionStackDump"))) {
	enableStackDumps (strtol (pEnvVarValue, (char **)NULL, 0) ? true : false);
    }
    if (IsntNil (pEnvVarValue = getenv ("VisionGCThreshold")) &&
	(iValue = (unsigned int)strtoul (pEnvVarValue, (char **)NULL, 0)) > 0) {
	setGCThreshold (iValue);
    }
    if (IsntNil (pEnvVarValue = getenv ("VisionGCSpread")) &&
	(iValue = (unsigned int)strtoul (pEnvVarValue, (char **)NULL, 0)) > 0) {
	setGCSpread (iValue);
    }
    
}

/**********************************************
 **********************************************
 *****                                    *****
 *****  VTransientServicesForBatchvision  *****
 *****                                    *****
 **********************************************
 **********************************************/

/**********************************
 **********************************
 *****  Exception Generation  *****
 **********************************
 **********************************/

/************************
 *****  Generators  *****
 ************************/

void VTransientServicesForBatchvision::vraiseApplicationException (
    char const *pMessageFormat, va_list pMessageArguments
) {
    raiseException (
	EC__UsageError,
	"Application Error: %s",
	vstring (pMessageFormat, pMessageArguments)
    );
}

void VTransientServicesForBatchvision::vraiseSystemException (
    char const *pMessageFormat, va_list pMessageArguments
) {
    raiseException (
	EC__UsageError,
	"System Error: %s",
	vstring (pMessageFormat, pMessageArguments)
    );
}

void VTransientServicesForBatchvision::vraiseUnimplementedOperationException (
    char const *pMessageFormat, va_list pMessageArguments
) {
    raiseException (
	EC__UnimplementedCase,
	"Unimplemented Operation: %s",
	vstring (pMessageFormat, pMessageArguments)
    );
}

void VTransientServicesForBatchvision::vraiseException (
    ERRDEF_ErrorCode xErrorCode, char const *pMessageFormat, va_list pMessageArguments
) {
    ERR_SignalFault (xErrorCode, vstring (pMessageFormat, pMessageArguments));
}


/*******************************
 *******************************
 *****    Stack Dumper     *****
 *******************************
 *******************************/

#if defined(sun) || defined(__linux__)

void VTransientServicesForBatchvision::enableStackDumps (bool bOnOff) {
    int fd;
    if (-1 == (fd = open ("/usr/bin/pstack", O_RDONLY, 0)))
	bOnOff = false;
    else
	close (fd);
    m_bStackDumpEnabled = bOnOff;
}

void VTransientServicesForBatchvision::dumpStack () {
    static char const *pStackDumpCommand = "/usr/bin/pstack %d > pstack.%d.%d";
    static char pCommandBuffer [256];
    if (m_bStackDumpEnabled) {
	int pid = getpid ();
	STD_sprintf (pCommandBuffer, pStackDumpCommand, pid, pid, m_iErrorCount++);
	system (pCommandBuffer);
    }
}
#else
void VTransientServicesForBatchvision::dumpStack () {
}
void VTransientServicesForBatchvision::enableStackDumps (bool bOnOff) {
    m_bStackDumpEnabled = false;
}
#endif

/*******************************
 *******************************
 *****  Memory Management  *****
 *******************************
 *******************************/

PrivateVarDef bool   g_bAttemptingToReclaimResources = false;
PrivateVarDef size_t g_sOriginalAllocationSize = 0;

/********************
 *****  malloc  *****
 ********************/

void *VTransientServicesForBatchvision::allocate (size_t cBytes) {
    if (cBytes == 0)
	cBytes = 1;

    VkStatus iStatus;
    // VMS malloc (and presumably realloc) will pass back a legitimate looking address for sizes
    // greater than that encoded in the if statement below. However when attempting to access memory
    // at the end of the allocation area (to set a container's end marker, for example) VMS will
    // signal an access violation. Since this encoded limit is far greater than any current legitimate
    // need for allocation size, we put this simple, if inelegant, guard in place without any machine
    // specific directives ...
    if (cBytes > 2013240000) {
	iStatus.MakeErrorStatus (EINVAL);
    }
    else do {
	V::U64 cTotalBytesNeeded = cBytes + UTIL_FreeListTotal + static_cast<V::U64> (ENVIR_SessionMemoryUse ()) +
				   VOutputBuffer::cellAllocatorCurrentAllocationAmount ();
#if defined (USING_SYS_RESOURCE_LIMITS)

    // As of early 2008, Linux does not enforce limitations on the heap size (limit datasize or ulimit -d)
    // To prevent individual batchvision sessions from consuming too much swap and RAM,
    // an explicit check has been added to ensure that the allocations which we already keep track of
    // do not exceed the resource limit. We are ignoring reallocs and some other minor allocations,
    // because keeping track of them would add runtime expense. We don't care if we are exact. We
    // just want to prevent a batchvision process from (accidentally) hogging system resources...

    // As of mid 2008, I am thinking this is actually a better way to maintain the limit than
    // relying on the unforgiving nature of the built-in resource limit. It has the advantage of allowing
    // error recovery/reporting to succeed when it otherwise might fail. So, now sun does it this way too!

	if ((cTotalBytesNeeded > m_sMemoryAllocationLimit) &&
	    // Allow allocations during error recovery and if cBytes is less
	    // than 4096 so that  error reporting has a chance to complete
	    !g_bAttemptingToReclaimResources && (cBytes > 4096) ) {
	    iStatus.MakeErrorStatus (ENOMEM);
	} else if (cTotalBytesNeeded > m_sMemoryAllocationLimit + 4096 * 100) {
	    // If we are too far beyond the limit, give up.
	    iStatus.MakeErrorStatus (ENOMEM);
	}
	else {
#endif
	    if (cTotalBytesNeeded > m_sGCThreshold) {
		if (!g_bAttemptingToReclaimResources) {
		    garbageCollected ();
		    cTotalBytesNeeded = cBytes + UTIL_FreeListTotal + static_cast<V::U64> (ENVIR_SessionMemoryUse ()) +
			VOutputBuffer::cellAllocatorCurrentAllocationAmount ();

		    m_sGCThreshold = V_Max (cTotalBytesNeeded + m_sGCSpread, m_sInitialGCThreshold);
		}
	    } else {
		V::U64 sGCThreshold = cTotalBytesNeeded + m_sGCSpread;
		if ( sGCThreshold < m_sGCThreshold) {
		    m_sGCThreshold = V_Max (sGCThreshold, m_sInitialGCThreshold);
		}
	    }
	    void *const result = ::malloc (cBytes);
	    if (result) {
#if defined(_WIN32)
		m_sTotalMemoryAllocation += _msize (result);
		if (m_sTotalMemoryAllocationHighWaterMark < m_sTotalMemoryAllocation)
		    m_sTotalMemoryAllocationHighWaterMark = m_sTotalMemoryAllocation;
#endif
		return result;
	    }
	    iStatus.MakeErrorStatus ();
#if defined (USING_SYS_RESOURCE_LIMITS)
	}
#endif
	if (!g_bAttemptingToReclaimResources)
	    g_sOriginalAllocationSize = cBytes;
    } while (
	!g_bAttemptingToReclaimResources
	&& ( garbageCollected ()
#if defined (sun) || defined (_WIN32) || defined (__linux__) || defined(__APPLE__)
	     // reclaiming mappings on VMS won't help.
	|| mappingsReclaimed ()
#endif
	)
    );

    double allocTotal = 0, mappingTotal = 0;
    M_AccumulateAllocationStatistics (&allocTotal, &mappingTotal);
    dumpStack ();
    if (g_bAttemptingToReclaimResources) {
	size_t sOrigBytes = g_sOriginalAllocationSize;
	g_sOriginalAllocationSize = 0;
	g_bAttemptingToReclaimResources = false;
	raiseException (
	    EC__MallocError,
	    "Private Memory Allocation Recovery Error\nOriginal Request Size: %u\nSize: %u, AT: %.0f, MT: %.0f, FL: %u LIO: %u\nDetails: %s",
	    sOrigBytes, cBytes, allocTotal, mappingTotal, UTIL_FreeListTotal,
	    VOutputBuffer::cellAllocatorCurrentAllocationAmount (),
	    iStatus.CodeDescription ()
	);
    }
    else
	raiseException (
	    EC__MallocError,
	    "Private Memory Allocation Error\nSize: %u, AT: %.0f, MT: %.0f, FL: %u LIO: %u\nDetails: %s",
	    cBytes, allocTotal, mappingTotal, UTIL_FreeListTotal,
	    VOutputBuffer::cellAllocatorCurrentAllocationAmount (),
	    iStatus.CodeDescription ()
	);

    return NilOf (void*);
}


/*********************
 *****  realloc  *****
 *********************/

void *VTransientServicesForBatchvision::reallocate (void *pMemory, size_t cBytes) {
    if (cBytes == 0)
	cBytes = 1;

    VkStatus iStatus;
    // VMS malloc (and presumably realloc) will pass back a legitimate looking address for sizes
    // greater than that encoded in the if statement below. However when attempting to access memory
    // at the end of the allocation area (to set a container's end marker, for example) VMS will
    // signal an access violation. Since this encoded limit is far greater than any current legitimate
    // need for allocation size, we put this simple, if inelegant, guard in place without any machine
    // specific directives ...
    if (cBytes > 2013240000) {
	iStatus.MakeErrorStatus (EINVAL);
    }
    else do {
#if defined(_WIN32)
	if (pMemory)
	    m_sTotalMemoryAllocation -= _msize (pMemory);
#endif
#if defined (USING_SYS_RESOURCE_LIMITS)
	if (cBytes + UTIL_FreeListTotal + static_cast<V::U64> (ENVIR_SessionMemoryUse ()) +
				VOutputBuffer::cellAllocatorCurrentAllocationAmount () >
		m_sMemoryAllocationLimit + 4096 * 100){
	    iStatus.MakeErrorStatus (ENOMEM);
	} else {
#endif
	    void *const result = ::realloc (pMemory, cBytes);
	    if (result) {
#if defined(_WIN32)
		m_sTotalMemoryAllocation += _msize (result);
		if (m_sTotalMemoryAllocationHighWaterMark < m_sTotalMemoryAllocation)
		    m_sTotalMemoryAllocationHighWaterMark = m_sTotalMemoryAllocation;
#endif
		return result;
	    }
	    iStatus.MakeErrorStatus ();
	    if (!g_bAttemptingToReclaimResources)
		g_sOriginalAllocationSize = cBytes;
#if defined (USING_SYS_RESOURCE_LIMITS)
	}
#endif
    } while (
	!g_bAttemptingToReclaimResources 
	&& (garbageCollected ()
#if defined (sun) || defined (_WIN32) || defined (__linux__) || defined (__APPLE__)
	     // reclaiming mappings on VMS won't help.
	|| mappingsReclaimed ()
#endif
	)
    );

    double allocTotal = 0, mappingTotal = 0;
    M_AccumulateAllocationStatistics (&allocTotal, &mappingTotal);
    dumpStack ();
    raiseException (
	EC__MallocError,
	"Private Memory Re-Allocation Error\nAddress: %08X\nSize: %u, AT: %.0f, MT: %.0f, FL: %u, LIO: %u, Details: %s",
	pMemory,
	cBytes, allocTotal, mappingTotal, UTIL_FreeListTotal, 
	VOutputBuffer::cellAllocatorCurrentAllocationAmount (),
	iStatus.CodeDescription ()
    );

    return NilOf (void*);
}


/******************
 *****  free  *****
 ******************/

void VTransientServicesForBatchvision::deallocate (void *pMemory) {
    if (pMemory) {
#if defined(_WIN32)
	m_sTotalMemoryAllocation -= _msize (pMemory);
#endif
	::free ((char *)pMemory);
    }
}


/*****************************************
 *****************************************
 *****  Message Formatting Services  *****
 *****************************************
 *****************************************/

int VTransientServicesForBatchvision::vdisplay (
    char const* pMessageFormat, va_list pMessageArguments
) {
    return IO_vnprintf (0, pMessageFormat, pMessageArguments);
}


/**********************************
 **********************************
 *****  Resource Reclamation  *****
 **********************************
 **********************************/

bool VTransientServicesForBatchvision::garbageCollected () {
    g_bAttemptingToReclaimResources = true;
    bool result = M_DisposeOfSessionGarbage (true);
    g_bAttemptingToReclaimResources = false;
    return result;
}

bool VTransientServicesForBatchvision::mappingsReclaimed () {
    g_bAttemptingToReclaimResources = true;
    bool result = M_ReclaimSegments () > 0;
    g_bAttemptingToReclaimResources = false;
    return result;
}

void VTransientServicesForBatchvision::logUsageInfo (char const *pFormat, ...) {
    V_VARGLIST (ap, pFormat);
    m_iUsageLog.vlog (pFormat, ap);
}

/**************************
 **************************
 *****  Notification  *****
 **************************
 **************************/

void VTransientServicesForBatchvision::setNSServer (char const *pServer) {
    m_iNSServer.setTo (pServer);
}
void VTransientServicesForBatchvision::setNSMessage (char const *pMsg) {
    m_iNSMessage.setTo (pMsg);
}


/**********************
 **********************
 *****            *****
 *****  Facility  *****
 *****            *****
 **********************
 **********************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (VTransientServices);
    FAC_FDFCase_FacilityDescription ("VTransientServices");
    default:
        break;
    }
}
