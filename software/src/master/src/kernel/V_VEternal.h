#ifndef V_VEternal_Interface
#define V_VEternal_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VFamilyValues.h"

/********************************************************************************************************************************

	A nasty little bug occurs for some compiler/linker/build/runtime combinations resulting in multiple invocations
	of the constructors for file scoped statics.  Currently, the problem is known to exist with:

	    - the Sun/Forte compiler/linker
	    - our use of 'gcc' on Linux, but only in the 'rake' online build.

	For systems known to have the problem, this class attempts to detect and suppress multiple constructor calls.

 ********************************************************************************************************************************/

#include "VConfig.h"
#if defined(sun) || (defined(VMS_LINUX_EXPLICIT_COMPAT) && defined(__linux__))

#define V_VEternal_PlatformConstructsStaticsMoreThanOnce

#endif

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    template <typename T> class VEternal{
	DECLARE_NUCLEAR_FAMILY(VEternal<T>);

    //****************
    //  class TBag wraps 'T' with the placement new/delete operators required by VEternal<T>:
    public:
	class TBag {
	public:
	    void* operator new (size_t sObject, void* pArea) {
		return pArea;
	    }
	    void operator delete (void* pObject, void* pArea) {
	    }
	    template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8> TBag (
		P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8
	    ) : m_iT (p1, p2, p3, p4, p5, p6, p7, p8) {
	    }
	    template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7> TBag (
		P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7
	    ) : m_iT (p1, p2, p3, p4, p5, p6, p7) {
	    }
	    template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6> TBag (
		P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6
	    ) : m_iT (p1, p2, p3, p4, p5, p6) {
	    }
	    template <typename P1, typename P2, typename P3, typename P4, typename P5> TBag (
		P1 p1, P2 p2, P3 p3, P4 p4, P5 p5
	    ) : m_iT (p1, p2, p3, p4, p5) {
	    }
	    template <typename P1, typename P2, typename P3, typename P4> TBag (
		P1 p1, P2 p2, P3 p3, P4 p4
	    ) : m_iT (p1, p2, p3, p4) {
	    }
	    template <typename P1, typename P2, typename P3> TBag (
		P1 p1, P2 p2, P3 p3
	    ) : m_iT (p1, p2, p3) {
	    }
	    template <typename P1, typename P2> TBag (
		P1 p1, P2 p2
	    ) : m_iT (p1, p2) {
	    }
	    template <typename P1> TBag (
		P1 p1
	    ) : m_iT (p1) {
	    }
	    TBag () {
	    }
	    operator T const& () const {
		return m_iT;
	    }
	    operator T& () {
		return m_iT;
	    }
	private:
	    T m_iT;
	};

    //****************
    //  Construction
    public:
	template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8> VEternal (
	    P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8
	) {
	    if (m_pTBag != tbagAddress ())
		m_pTBag  = new(tbagAddress()) TBag (p1, p2, p3, p4, p5, p6, p7, p8);
	}
	template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7> VEternal (
	    P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7
	) {
	    if (m_pTBag != tbagAddress ())
		m_pTBag  = new(tbagAddress()) TBag (p1, p2, p3, p4, p5, p6, p7);
	}
	template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6> VEternal (
	    P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6
	) {
	    if (m_pTBag != tbagAddress ())
		m_pTBag  = new(tbagAddress()) TBag (p1, p2, p3, p4, p5, p6);
	}
	template <typename P1, typename P2, typename P3, typename P4, typename P5> VEternal (
	    P1 p1, P2 p2, P3 p3, P4 p4, P5 p5
	) {
	    if (m_pTBag != tbagAddress ())
		m_pTBag  = new(tbagAddress()) TBag (p1, p2, p3, p4, p5);
	}
	template <typename P1, typename P2, typename P3, typename P4> VEternal (
	    P1 p1, P2 p2, P3 p3, P4 p4
	) {
	    if (m_pTBag != tbagAddress ())
		m_pTBag  = new(tbagAddress()) TBag (p1, p2, p3, p4);
	}
	template <typename P1, typename P2, typename P3> VEternal (
	    P1 p1, P2 p2, P3 p3
	) {
	    if (m_pTBag != tbagAddress ())
		m_pTBag  = new(tbagAddress()) TBag (p1, p2, p3);
	}
	template <typename P1, typename P2> VEternal (
	    P1 p1, P2 p2
	) {
	    if (m_pTBag != tbagAddress ())
		m_pTBag  = new(tbagAddress()) TBag (p1, p2);
	}
	template <typename P1> VEternal (
	    P1 p1
	) {
	    if (m_pTBag != tbagAddress ())
		m_pTBag  = new(tbagAddress()) TBag (p1);
	}
	VEternal () {
	    if (m_pTBag != tbagAddress ())
		m_pTBag  = new(tbagAddress()) TBag ();
	}

    //  Destruction
    public:
	~VEternal () {
	}

    //  Access
    public:
	operator T const& () const {
	    return *tbagAddress();
	}
	operator T& () {
	    return *tbagAddress();
	}
	T * operator-> () {
	    T & content = *tbagAddress();
	    return &content;
	}
	T const * operator-> () const {
	    T const & content = *tbagAddress();
	    return &content;
	}

    private:
	TBag const* tbagAddress () const{
	    return reinterpret_cast<TBag const*>(m_iT.asMemory);
	}
	TBag* tbagAddress () {
	    return reinterpret_cast<TBag*>(m_iT.asMemory);
	}

    //  State
    private:
	TBag* m_pTBag;	// ... used to record the address of the TBag.  When != the tbag address, indicates
			//     that this object has NOT been constructed.  This fact is also used to prevent
			//     re-construction of the eternal on platforms that have that bug.
	union {		// ... try to force the most restrictive alignment required by the target processor
	    void* asAddress;
	    __int64 as64;
	    double asDouble;
	    char asMemory[sizeof (TBag)];
	} m_iT;
    };
}


#endif
