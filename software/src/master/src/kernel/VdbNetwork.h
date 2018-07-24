/*****  Vision Network DB Handle Interface  *****/
#ifndef VdbNetwork_Interface
#define VdbNetwork_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VReference.h"

/**************************
 *****  Declarations  *****
 **************************/

class VkStatus;

#if defined(_WIN32) 
#include <assert.h>

#elif defined(__VMS)
#include "VkMapOf.h"  // omit:Linux omit:SunOS
#include "V_VBlob.h"  // omit:Linux omit:SunOS

namespace V {
    namespace OS {
	typedef V::VBlob LockName_t;
	class ResourceDomain;

	V_API bool ConvertUnixFileNameToVms (VString &rConvertedName, char const *pFileName);
	V_API bool ConvertVmsFileNameToUnix (VString &rConvertedName, char const *pFileName);
    }
}
#endif


/*************************
 *****  Definitions  *****
 *************************/

enum VdbNetworkLockOperation {
    VdbNetworkLockOperation_Lock,
    VdbNetworkLockOperation_TLock,
    VdbNetworkLockOperation_Unlock
};

class V_API VdbNetwork : public VTransient {
    DECLARE_FAMILY_MEMBERS (VdbNetwork, VTransient);

//  Aliases
 public:
    typedef V::VString VString;

//  Max Lock Count (same as max space count)
public:
    //static const size_t s_maxlock = 1024;  // doesn't work on MSVC6
    enum { s_maxlock = 1024 };

//  Construction
private:
    void ResolveNetworkLink ();
public:
    VdbNetwork (char const *pNDFPathName, char *pOSDPathName, int hNDF);

    ~VdbNetwork();

    void SetNDFHandleTo (int hNDF) {
	m_hNDF = hNDF;
    }
    void SetNJFHandleTo (int hNJF) {
	m_hNJF = hNJF;
    }

//  Access
public:
    int NDFHandle () const {
	return m_hNDF;
    }
    char const* NDFPathName () const {
	return m_pNDFPathName;
    }

    int NJFHandle () const {
	return m_hNJF;
    }
    char const* NJFPathName () const;

    char const* NLFPathName () const;
    char const* OSDPathName () const {
	return m_pOSDPathName;
    }

//  I/O
public:
//    off_t Seek (off_t offset);
//    void Get (void *pBuffer, size_t sBuffer, bool bCheckingSize);

//  Locking
public:
    void Lock (
	int xLock, VdbNetworkLockOperation xLockOperation, VkStatus *pStatusReturn
    );

#ifdef __VMS
private:
    void VMS_Lock(unsigned short xLock, VdbNetworkLockOperation xLockOperation, VkStatus *pStatusReturn);
    void VMS_Unlock(short xLock);
#endif

//  State
private:
    char const*			m_pNDFPathName;
    char*			m_pOSDPathName;
    int				m_hNDF;
    int				m_hNJF;

#ifdef __VMS
    typedef V::OS::ResourceDomain ResourceDomain;
    VReference<ResourceDomain>	m_pResourceDomain;

    typedef V::OS::LockName_t LockName_t;
    LockName_t			m_lockprefix;

    typedef VkMapOf<unsigned short, unsigned short, unsigned short, unsigned int> LockMap_t;
    LockMap_t			m_lockmap;

#elif defined(_WIN32)
    template <size_t Nmax> class LockBitmap {
	typedef unsigned char byte_t;

	byte_t m_bitmap[(Nmax + CHAR_BIT - 1) / CHAR_BIT];

      public:
	LockBitmap()
	    { memset(m_bitmap, 0, sizeof(m_bitmap)); }

	bool test(size_t bit)
	{
	    assert(bit < Nmax);
	    const byte_t& byte = m_bitmap[bit / CHAR_BIT];
	    const size_t subbit = bit % CHAR_BIT;
	    return byte & (1 << subbit) ? true : false;
	}

	void set(size_t bit, bool val = true)
	{
	    assert(bit < Nmax);
	    byte_t& byte = m_bitmap[bit / CHAR_BIT];
	    const size_t subbit = bit % CHAR_BIT;
	    if (val)
		byte |= static_cast<byte_t>(1 << subbit);
	    else
		byte &= static_cast<byte_t>(~(1 << subbit));
	}

	void clear(size_t bit)
	    { set(bit, false); }

    };  // class LockBitmap

    HANDLE			m_hLockFile;
    LockBitmap<s_maxlock>	m_lockbitmap;

#endif  // _WIN32 || __VMS
};


#endif
