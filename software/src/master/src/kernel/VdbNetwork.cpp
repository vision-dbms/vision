/*****  Vision Network Handle Implementation  *****/

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

#include "VdbNetwork.h"

/************************
 *****  Supporting  *****
 ************************/

#include "VkStatus.h"

#ifdef __VMS

#include "boost_static_assert.h"

#ifndef __NEW_STARLET
#define __NEW_STARLET
#endif

#ifndef _LARGEFILE
#define _LARGEFILE
#endif

#define V_VMS_LINUX_COMPAT_DEBUGGING_TOOLS 1
#define V_USING_CUSTOM_DESCRIPTOR 1
#include "VConfig.h"

#include "V_VSpinlock.h"	// omit:Linux omit:SunOS
#include "V_VString.h"		// omit:Linux omit:SunOS
#include "V_VThreadSpecific.h"	// omit:Linux omit:SunOS


#ifndef __NEW_STARLET
#define __NEW_STARLET
#endif

#ifndef _LARGEFILE
#define _LARGEFILE
#endif

#include <starlet.h>
#include <lksbdef.h>
#include <lckdef.h>
#include <ssdef.h>
#include <rsdmdef.h>
#include <uicdef.h>
#include <unixlib.h>
#include <stat.h>
#include <psldef.h>  // PSL$C_USER
#include <efndef.h>  // EFN$C_ENF


using namespace boost;


#endif	// __VMS


/********************************************
 ********************************************
 *****                                  *****
 *****  V::OS::Convert<X>FileNameTo<Y>  *****
 *****                                  *****
 ********************************************
 ********************************************/

#ifdef __VMS
template class V::VThreadSpecificBinding_<VString*>;

namespace {
  // the decc$to_vms interface leaves something to be desired...
  static V::VThreadSpecific_<VString*> g_pConvertedName;	// thread specific global.

  extern "C" int to_vms_callback(char* name, int flag) {
    assert (g_pConvertedName != 0);

    if (flag == DECC$K_FILE) {
	g_pConvertedName.value()->setTo(name);
	return true;
    }
    return false;
  }

  extern "C" int from_vms_callback(char *name) {
    assert (g_pConvertedName != 0);

    g_pConvertedName.value()->setTo(name);
    return true;
  }
}

namespace V {
    namespace OS {
	bool ConvertUnixFileNameToVms (VString &rConvertedName, char const *pFileName) {
	    g_pConvertedName = &rConvertedName;
	    return decc$to_vms(pFileName, &to_vms_callback, 0, 0) == 1;
	}

	bool ConvertVmsFileNameToUnix (VString &rConvertedName, char const *pFileName) {
	    g_pConvertedName = &rConvertedName;
	    return decc$from_vms(pFileName, &from_vms_callback, 0) == 1;
	}

	bool ConvertFileNameToVms (VString &rConvertedName, char const *pFileName) {
	//  Attempt the translation...
	    if (ConvertUnixFileNameToVms (rConvertedName, pFileName))
		return true;

	//  Look for the target format...
	    if (ConvertVmsFileNameToUnix (rConvertedName, pFileName)) {
		rConvertedName.setTo (pFileName);
		return true;
	    }

	//  Fail...
	    return false;
	}

	bool ConvertFileNameToUnix (VString &rConvertedName, char const *pFileName) {
	//  Attempt the translation...
	    if (ConvertVmsFileNameToUnix (rConvertedName, pFileName))
		return true;

	//  Look for the target format...
	    if (ConvertUnixFileNameToVms (rConvertedName, pFileName)) {
		rConvertedName.setTo (pFileName);
		return true;
	    }

	//  Fail...
	    return false;
	}
    }
}


/***********************************
 ***********************************
 *****                         *****
 *****  V::OS::ResourceDomain  *****
 *****                         *****
 ***********************************
 ***********************************/

class V::OS::ResourceDomain : public VReferenceable {
    DECLARE_VIRTUAL_RTT (V::OS::ResourceDomain, VReferenceable);	// No 'deleteThis'

//  id_t
public:
    typedef unsigned int id_t;

//  Instance Map
public:
    typedef VkMapOf<VString, VString const&, char const *, Pointer> InstanceMap_t;
    typedef V::VSpinlock InstanceMapLock_t;
private:
    static InstanceMap_t	g_iInstanceMap;
    static InstanceMapLock_t	g_iInstanceMapLock;

//  Instance
public:
    static Reference Instance (VString const &rDomainName);

//  Construction
private:
    ResourceDomain (VString const &rDomainName, id_t xDomainId);

//  Destruction
private:
    ~ResourceDomain () {
    }
    virtual void deleteThis ();

//  Access
public:
    VString const &domainName () const {
	return m_iDomainName;
    }
    id_t domainId () const {
	return m_xDomainId;
    }

//  State
private:
    VString const	m_iDomainName;
    id_t		m_xDomainId;
};


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_VIRTUAL_RTT (V::OS::ResourceDomain);

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

V::OS::ResourceDomain::InstanceMap_t	 V::OS::ResourceDomain::g_iInstanceMap;
V::OS::ResourceDomain::InstanceMapLock_t V::OS::ResourceDomain::g_iInstanceMapLock;

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

V::OS::ResourceDomain::ResourceDomain (
    VString const &rDomainName, id_t m_xDomainId
) : m_iDomainName (rDomainName), m_xDomainId (m_xDomainId) {
    //  'ResourceDomain::Instance' constructs all instances under cover of the instance map lock...
    unsigned int xMapEntry;
    if (g_iInstanceMap.Insert (rDomainName, xMapEntry))
	g_iInstanceMap[xMapEntry].setTo (this);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

void V::OS::ResourceDomain::deleteThis () {
    InstanceMapLock_t::Claim iLockClaim (g_iInstanceMapLock);

    //  Don't destroy what doesn't belong to us exclusively...
    if (referenceCount () == 0) {
	unsigned int xMapEntry;
	if (g_iInstanceMap.Locate (m_iDomainName, xMapEntry) && g_iInstanceMap[xMapEntry] == this) {
	    g_iInstanceMap.Delete (m_iDomainName);

#pragma __pointer_size __save
#pragma __pointer_size __short
	    id_t xDomainId = m_xDomainId;
	    sys$set_resource_domain(RSDM$_LEAVE, static_cast<unsigned int*>(&xDomainId), 0, 0, 0, 0);
#pragma __pointer_size __restore
	}
	delete this;
    }
}


/**********************
 **********************
 *****  Instance  *****
 **********************
 **********************/

//  Returning a 'Reference' allows 'deleteThis' to detect that it's in a race with this routine and back off...
V::OS::ResourceDomain::Reference V::OS::ResourceDomain::Instance (VString const &rDomainName) {
    InstanceMapLock_t::Claim iLockClaim (g_iInstanceMapLock);

    unsigned int xMapEntry;
    if (g_iInstanceMap.Locate (rDomainName, xMapEntry))
	return Reference (g_iInstanceMap[xMapEntry].referent ());

    dsc_descriptor_s domname (const_cast<char*>(rDomainName.content ()), rDomainName.length ());
      // can't make this const, because sys$asctoid() is not const-correct

    BOOST_STATIC_ASSERT(sizeof(unsigned int) == sizeof(UICDEF));
    union {
	unsigned int rawuic;
	UICDEF uic;
    };
    unsigned int stat = sys$asctoid(&domname, &rawuic, 0);
    if (!(stat & 1)) {
	VString msg ("unable to resolve resource domain name to id, status: ", false);
	msg << stat;
	VERIFY_EX2(stat & 1, msg);
	return Reference ();
    }

    id_t xDomainId = 0;
    const unsigned int uicgrp = uic.uic$w_grp;  // this part is made *real* clear in the docs..
#pragma __pointer_size __save
#pragma __pointer_size __short
    stat = sys$set_resource_domain(RSDM$_JOIN_DOMAIN, static_cast<unsigned int*>(&xDomainId), uicgrp, 0, 0, 0);
#pragma __pointer_size __restore
    if (!(stat & 1)) {
	VString msg ("user could not join resource domain, status: ", false);
	msg << stat;
	VERIFY_EX2(stat & 1, msg);
	return Reference ();
    }
    return Reference (new ResourceDomain (rDomainName, xDomainId));
}
#endif // __VMS


/******************************
 ******************************
 *****                    *****
 *****  class VdbNetwork  *****
 *****                    *****
 ******************************
 ******************************/

/*********************
 *****  Locking  *****
 *********************/

/*---------------------------------------------------------------------------
 *****  Internal utility to perform a lock operation.
 *
 *  Arguments:
 *	this			- the address of a network handle instance.
 *	xLock			- the lock to select.  A negative 'xLock'
 *				  selects all possible locks.
 *	xOperation			- the lock operation to be performed.
 *	pStatusReturn		- the address of a status object initialized to
 *				  a Completed, Blocked (i.e., TLOCK failed), or
 *				  Failed status.
 *
 *****/
void VdbNetwork::Lock (
    int xLock, VdbNetworkLockOperation xLockOperation, VkStatus *pStatusReturn
) {
#ifdef __VMS

/**************************
 *  Alpha Implementation  *
 **************************/

    VMS_LOCKING_CONTENTION_TESTING_FAILGUARD_START 

	if (m_pResourceDomain.isNil ())  // could not attach to the correct resource domain
	    return;

	if (xLock > 0 && xLock >= s_maxlock)
	    return;

	switch (xLockOperation) {
	  case VdbNetworkLockOperation_Lock:
	  case VdbNetworkLockOperation_TLock:
	    if (xLock < 0)
		return;

	    VMS_Lock(static_cast<unsigned short>(xLock), xLockOperation, pStatusReturn);
	    break;

	  case VdbNetworkLockOperation_Unlock:
	    VMS_Unlock(static_cast<unsigned short>(xLock));
	    pStatusReturn->MakeSuccessStatus();
	    break;

	  default:  // should never happen - bomb out
	    throw logic_error("unknown lock operation");
	}  // switch (xLockOperation)

    VMS_LOCKING_CONTENTION_TESTING_FAILGUARD_END


#elif defined(_WIN32)

/**************************
 *  Win32 Implementation  *
 **************************/

    OVERLAPPED	iOrigin;
    DWORD	sFullExtent;
    DWORD	sThisExtent;

/*****  Validate the lock number, ...  *****/
    if (xLock > 0 && xLock >= s_maxlock) {
	pStatusReturn->MakeFailureStatus ();
	return;
    }

/*****  Open or create the lock file, ...  *****/
    if (FileHandleIsInvalid (m_hLockFile)) {
	char iPathBuffer[1024];
	sprintf (iPathBuffer, "%s.LOCK", m_pNDFPathName);

	m_hLockFile = CreateFile (
	    iPathBuffer,
	    GENERIC_READ,
	    FILE_SHARE_READ | FILE_SHARE_WRITE,
	    NULL,
	    OPEN_ALWAYS,
	    FILE_ATTRIBUTE_NORMAL,
	    NULL
	);
    }
    if (FileHandleIsInvalid (m_hLockFile)) {
	pStatusReturn->MakeErrorStatus ();
	return;
    }

    iOrigin.Internal		=
    iOrigin.InternalHigh	= 0;
    iOrigin.Offset		= xLock;
    iOrigin.OffsetHigh		= 0;
    iOrigin.hEvent		= NULL;

    switch (xLockOperation) {
    case VdbNetworkLockOperation_Lock:
	if (xLock < 0)
	    pStatusReturn->MakeFailureStatus ();
	else if (m_lockbitmap.test(xLock))
	    pStatusReturn->MakeSuccessStatus ();
	else if (LockFileEx (m_hLockFile, LOCKFILE_EXCLUSIVE_LOCK, 0, 1, 0, &iOrigin)) {
	    m_lockbitmap.set(xLock);
	    pStatusReturn->MakeSuccessStatus ();
	}
	else pStatusReturn->MakeErrorStatus ();
	break;

    case VdbNetworkLockOperation_TLock:
	if (xLock < 0)
	    pStatusReturn->MakeFailureStatus ();
	else if (m_lockbitmap.test(xLock))
	    pStatusReturn->MakeSuccessStatus ();
	else if (
	    LockFileEx (
		m_hLockFile,
		LOCKFILE_EXCLUSIVE_LOCK | LOCKFILE_FAIL_IMMEDIATELY,
		0,
		1,
		0,
		&iOrigin
	    )
	)
	{
	    m_lockbitmap.set(xLock);
	    pStatusReturn->MakeSuccessStatus ();
	}
	else switch (GetLastError ()) {
	case ERROR_LOCK_VIOLATION:
	    pStatusReturn->MakeBlockedStatus ();
	    break;
	default:
	    pStatusReturn->MakeErrorStatus ();
	    break;
	}
	break;

    case VdbNetworkLockOperation_Unlock:
	pStatusReturn->MakeSuccessStatus ();
	if (xLock < 0) {
	    iOrigin.Offset	= 0;
	    sFullExtent		= s_maxlock;
	}
	else {
	    iOrigin.Offset	= xLock;
	    sFullExtent		= 1;
	}
	sThisExtent = 0;
	while (sFullExtent-- > 0) {
	    if (!m_lockbitmap.test(iOrigin.Offset)) {}
	    else if (UnlockFileEx (m_hLockFile, 0, 1, 0, &iOrigin))
		m_lockbitmap.clear(iOrigin.Offset);
	    else {
		pStatusReturn->MakeFailureStatus ();
		return;
	    }
	    iOrigin.Offset++;
	}
	break;

    default:
	pStatusReturn->MakeFailureStatus ();
	break;
    }


#else
/*************************
 *  Unix Implementation  *
 *************************/

    int xOperation;
    switch (xLockOperation) {
    case VdbNetworkLockOperation_Lock:
	xOperation = F_LOCK;
	break;
    case VdbNetworkLockOperation_TLock:
	xOperation = F_TLOCK;
	break;
    case VdbNetworkLockOperation_Unlock:
	xOperation = F_ULOCK;
	break;
    default:
	pStatusReturn->MakeFailureStatus ();
	return;
    }

    if (lseek (m_hNDF, xLock < 0 ? 0 : xLock, SEEK_SET) < 0) {
	pStatusReturn->MakeErrorStatus ();
	return;
    }

    if (0 == lockf (m_hNDF, xOperation, xLock < 0 ? 0 : 1)) {
	pStatusReturn->MakeSuccessStatus ();
	return;
    }

    if (F_TLOCK == xOperation && EACCES == errno) {
	pStatusReturn->MakeBlockedStatus ();
	return;
    }

    pStatusReturn->MakeErrorStatus ();
#endif
}


//-------------
// ALPHA CODE

#ifdef __VMS

namespace {
    // global resource domain accessible by inhouse & clients
    // (also used for FCD):
    const VString s_dblock_domain ("DBLOCK", false);

// -----
// LOCK NAME SUPPORT

    typedef V::OS::LockName_t LockName_t;

    const size_t s_maxlocknamelen = 31;  // as per VMS
    const size_t s_locktaglen = sizeof(unsigned short);  // 10 bits to represent 0-1024 unique lock ids
    const size_t s_maxlockprefixlen = s_maxlocknamelen - s_locktaglen;


    #ifdef VMS_LOCKING_CONTENTION_TESTING
    VMS_LOCKING_CONTENTION_TESTING_HELPER 
    #endif

    bool GetLockPrefixForVMSName(LockName_t &rLockPrefix, VString const &vmsname) {
	if (vmsname.isEmpty())
	    return false;

	struct stat stbuf;
	if (stat(vmsname, &stbuf) < 0)
	    return false;

	const char prefix[] = "VIS_";
	const size_t preflen = sizeof(prefix) - 1;
	const size_t devlen = strlen(stbuf.st_dev);
	const size_t cursize = (preflen + devlen + sizeof(stbuf.st_ino));
	//                         4    +  <=15  +       6
	//    <= 25..
	assert(cursize <= s_maxlockprefixlen);
	if (cursize > s_maxlockprefixlen)  // should/must never happen
	    throw logic_error("lock prefix length too long");

	rLockPrefix.storage (cursize);
	rLockPrefix.setContentBoundsTo (0, 0);

	rLockPrefix.append (prefix, preflen);
	rLockPrefix.append (stbuf.st_dev, devlen);
	rLockPrefix.append (&stbuf.st_ino, sizeof(stbuf.st_ino));

#ifdef VMS_LOCKING_CONTENTION_TESTING
VMS_LOCKING_CONTENTION_TESTING_LOGGING;
#endif // VMS_LOCKING_CONTENTION_TESTING

	return true;
    }

    bool GetLockPrefixForUnixName(LockName_t &rLockPrefix, const char* pFileName) {
	VString vmsname;
	return V::OS::ConvertFileNameToVms (vmsname, pFileName) && GetLockPrefixForVMSName(rLockPrefix, vmsname);
    }

    void GetLockName(LockName_t &rLockName, LockName_t const &prefix, unsigned short xLock) {
	BOOST_STATIC_ASSERT(sizeof(xLock) == s_locktaglen);
	assert(prefix.contentSize() > 0);

	rLockName.clear ();
	rLockName.storage (prefix.contentSize() + sizeof(xLock));

	rLockName.append (prefix);
	rLockName.append (&xLock, sizeof(xLock));

#ifdef VMS_LOCKING_CONTENTION_TESTING
VMS_LOCKING_CONTENTION_TESTING_LOGGING2; 
#endif

	if (rLockName.contentSize() > s_maxlocknamelen)  // should/must never happen
	    throw logic_error("lock name length too long");
    }

    void VMS_Unlock_one(unsigned int lockid) {
	assert(lockid != 0);
	const unsigned int status = sys$deq(lockid, 0, PSL$C_USER, 0);
	VERIFY_EX2(status & 1, "sys$deq failed in VMS_Unlock");
    }
}

void VdbNetwork::VMS_Lock(unsigned short xLock, VdbNetworkLockOperation xLockOp, VkStatus* pStatusReturn) {
    assert(xLock < s_maxlock);

    unsigned int xLockEntry;
    if (m_lockmap.Locate (xLock, xLockEntry)) {
	pStatusReturn->MakeSuccessStatus();
	return;
    }

    // can't make this const, because descriptors are not const-correct
    LockName_t lockname;
    GetLockName(lockname, m_lockprefix, xLock);

    const bool trylock = (xLockOp == VdbNetworkLockOperation_TLock);
    const unsigned int lockflags = LCK$M_SYNCSTS | (trylock ? LCK$M_NOQUEUE : 0);

    V::OS::dsc_descriptor_s locknamedesc(lockname.contentArea (), lockname.contentSize ());
    LKSB lksb = { 0 };

    const unsigned int status = sys$enqw(EFN$C_ENF, LCK$K_EXMODE, &lksb, lockflags,
					 &locknamedesc, 0, 0, 0, 0, PSL$C_USER, m_pResourceDomain->domainId ());
    if ((status & 1) && (lksb.lksb$w_status & 1)) {
	const unsigned int lockid = lksb.lksb$l_lkid;
	assert(lockid != 0);

	// release if we can't add the value to the map
	pStatusReturn->MakeErrorStatus (status);

        VMS_LOCKING_CONTENTION_TESTING_UNLOCKGUARD_START 
	    m_lockmap.Insert (xLock, xLockEntry);
	    m_lockmap[xLockEntry] = lockid;

	    pStatusReturn->MakeSuccessStatus();
        VMS_LOCKING_CONTENTION_TESTING_UNLOCKGUARD_END 

	return;
    }  // if ((status & 1) && (lksb.lksb$w_status & 1))

    // else, not locked:
    if (trylock && status == SS$_NOTQUEUED)
	pStatusReturn->MakeBlockedStatus();
    else
	pStatusReturn->MakeErrorStatus(status);
}

void VdbNetwork::VMS_Unlock(short xLock) {
    assert(xLock < 0 || xLock < s_maxlock);

    unsigned int xLockEntry = 0;
    if (xLock < 0) {
	unsigned int xLockLimit = m_lockmap.cardinality ();
	NothrowRegion {
	    while (xLockEntry < xLockLimit) {
		VMS_Unlock_one(m_lockmap[xLockEntry++]);
	    }
	    m_lockmap.DeleteAll ();
	}
    }
    else if (m_lockmap.Locate (xLock, xLockEntry)) {
	NothrowRegion {
	    VMS_Unlock_one(m_lockmap[xLockEntry]);
	    m_lockmap.Delete (xLock);
	}
    }
}
#endif // __VMS


/*******************
 *****  Query  *****
 *******************/

char const* VdbNetwork::NJFPathName () const {
    static char iPathBuffer[1024];
    sprintf (iPathBuffer, "%s.JOURNAL", m_pNDFPathName);
    return iPathBuffer;
}

char const* VdbNetwork::NLFPathName () const {
    static char iPathBuffer[1024];
    sprintf (iPathBuffer, "%s.ERRORS", m_pNDFPathName);
    return iPathBuffer;
}


/**************************
 *****  Construction  *****
 **************************/

void VdbNetwork::ResolveNetworkLink () {
    char  pLinkBuffer[1024];
    sprintf (pLinkBuffer, "%s.OSDPATH", m_pNDFPathName);

#if defined(_WIN32)
    FILE *pLinkStream = fopen (pLinkBuffer, "rt");
#else
    FILE *pLinkStream = fopen (pLinkBuffer, "r");
#endif

/*****  If NDF.OSDPATH doesn't exist, assume the directory in which the NDF lives, ... *****/
    if (IsNil (pLinkStream)) {
	char const* pLastSlash = strrchr (m_pNDFPathName, '/');
	if (!pLastSlash)
	    strcpy (m_pOSDPathName, "");
	else {
	    size_t const sDirname = static_cast<size_t>(pLastSlash - m_pNDFPathName);
	    strncpy (m_pOSDPathName, m_pNDFPathName, sDirname);
	    m_pOSDPathName[sDirname] = '\0';
	}
	return;
    }

/*****  ... otherwise, read the OSDPathName from NDF.OSDPATH as always:  *****/
    if (IsntNil (fgets (pLinkBuffer, sizeof (pLinkBuffer), pLinkStream))) {
	char *pNewLine = strchr (pLinkBuffer, '\n');
	if (pNewLine)
	    *pNewLine = '\0';
	strcpy (m_pOSDPathName, pLinkBuffer);
    }

    fclose (pLinkStream);
}

VdbNetwork::VdbNetwork (
    char const *pNDFPathName, char *pOSDPathName, int hNDF
) : m_pNDFPathName (pNDFPathName), m_pOSDPathName (pOSDPathName), m_hNDF (hNDF), m_hNJF (-1)
#ifdef __VMS
     , m_pResourceDomain (ResourceDomain::Instance (s_dblock_domain))
#elif defined(_WIN32)
     , m_hLockFile(INVALID_HANDLE_VALUE)
#endif
{
/*****  Resolve the network link, ...  *****/
    ResolveNetworkLink ();

#ifdef __VMS
    GetLockPrefixForUnixName (m_lockprefix, m_pNDFPathName);
    assert(m_lockprefix.contentSize() > 0 && m_lockprefix.contentSize() <= s_maxlockprefixlen);
#endif
}

// --------

VdbNetwork::~VdbNetwork() {
}
