/*****  Vision Kernel File Mapping Interface  *****/
#ifndef VkMemory_Interface
#define VkMemory_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "V_VMM.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

typedef int (__cdecl *VkMemoryTraceCallback) (char const *format, ...);

typedef void (*VkMemoryErrorCallback) (char const* pMessage);

class V_API VkMemory : public VTransient {
    DECLARE_FAMILY_MEMBERS (VkMemory, VTransient);

//  Aliases
public:
    typedef V::VMM::Space AddressSpace;

//  Enumerations
public:
    enum Share {
	Share_None		= 0,
	Share_Private		= 1,
	Share_Group		= 2,
	Share_GroupPrivate	= Share_Group + Share_Private,
	Share_System		= 4,
	Share_SystemPrivate	= Share_System + Share_Private,
	Share_Shared		= Share_System + Share_Group,
	Share_Any		= Share_System + Share_Group + Share_Private
    };	// combinations use most global sharing level that succeeds.

    enum MapStatus {
	Success, TryAgain, Failed
    };

//  Globals
private:
    static unsigned int g_cMappedFiles;
public:
    static unsigned int MappedFileCount () {
	return g_cMappedFiles;
    }

//  GRM Management
public:
    static void StartGRM (
	char const		*callerName,
	char const		*logFacility = 0,
	VkMemoryTraceCallback	traceFn = 0,
	VkMemoryErrorCallback	errorFn = 0
    );
    static void StopGRM ();

//  Construction
public:
    void Initialize () {
	m_pRegion = 0; m_sRegion = 0;
#ifdef __VMS
	m_sMapping = 0;
	m_xChannel = 0;
#endif
    }
    VkMemory () {
	Initialize ();
    }

//  Destruction
public:
    void Destroy ();

    ~VkMemory () {
	Destroy ();
    }

//  Access
public:
    void *RegionAddress () const {
	return m_pRegion;
    }
    size_t RegionSize () const {
	return m_sRegion;
    }

//  Update
public:
    bool Allocate (size_t size, bool shared);
protected:
    MapStatus Map (
	char const *pPathName, bool fReadOnly, size_t sMultiplier, size_t sIncrement, Share xShare
    );
public:
    bool Map (char const *fileName, size_t sMultiplier, size_t sIncrement, Share xShare);
    bool Map (char const *fileName, bool readOnly, Share xShare);

    bool MapNew (char const *pPathName, size_t sObject, Share xShare);

    bool Refresh ();

    bool Sync ();

    void SetRegionSizeTo (size_t sMapping) {
	if (!m_pRegion)
	    m_sRegion = sMapping;
    }

//  State
protected:
    void* 		m_pRegion;
    size_t		m_sRegion;
#if defined(__VMS)
    AddressSpace	m_iSpace;
    unsigned __int64	m_sMapping;
    unsigned short	m_xChannel;	//  Must be retained for the duration of the mapping for private file sections.
#elif defined(_WIN32)
    HANDLE		m_iHandle;
#endif
};

/**********************
 **********************
 *****  Behavior  *****
 **********************
 **********************/

#if defined(__hp9000s800) || defined(__hp9000s700)

PublicFnDef int VkMemory_GRMRequestCount ();

#endif


#endif
