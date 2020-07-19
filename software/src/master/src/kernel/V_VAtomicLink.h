#ifndef V_VAtomicLink_Interface
#define V_VAtomicLink_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VAtomicMemoryOperations_.h"

/***********************
 *****  Templates  *****
 ***********************/

namespace V {
    /*********************************
     *----  class HazardPointer  ----*
     *********************************/
    class HazardPointer {
    public:
	HazardPointer () : m_pHazard (0) {
	}
	~HazardPointer () {
	}
	void setTo (void *pHazard) {
	    m_pHazard = pHazard;
	}
	void reset () {
	    m_pHazard = 0;
	}
	void *value () const {
	    return m_pHazard;
	}
    private:
	void *m_pHazard;
    };

    /***********************************
     *----  class NoHazardPointer  ----*
     ***********************************/
    class NoHazardPointer {
    public:
	NoHazardPointer () {
	}
	~NoHazardPointer () {
	}
	void setTo (void *pHazard) {
	}
	void reset () {
	}
    };

    /**********************************************************
     *----  template <class Pointer_T> class VAtomicLink  ----*
     **********************************************************/

    template <class Pointer_T> class VAtomicLink : public Pointer_T {
	DECLARE_FAMILY_MEMBERS (VAtomicLink<Pointer_T>, Pointer_T);

    //  Aliases
    public:
	typedef typename Pointer_T::ReferencedClass ReferencedClass;
	typedef typename Pointer_T::LinkMember LinkMember;

    //  Construction
    public:
	VAtomicLink (ReferencedClass *pReferent) : BaseClass (pReferent) {
	}
	VAtomicLink (ThisClass const &rOther) : BaseClass (rOther) {
	}
	VAtomicLink () {
	}

    //  Destruction
    public:
	~VAtomicLink () {
	}

    //  Operations
    public:
	void interlockedPush (ReferencedClass *pFirst, LinkMember pLinkMember) {
	    V::VAtomicMemoryOperations::MemoryBarrierProduce ();

	    ReferencedClass *&rpFirstLink = (pFirst->*pLinkMember).m_pReferent;
	    do {
		(pFirst->*pLinkMember).setTo (m_pReferent);
	    } while (!BaseClass::interlockedSetIf (pFirst, rpFirstLink));
	}
	bool interlockedPop (BaseClass &rpFirst, LinkMember pLinkMember) {
	    NoHazardPointer pHazard;
	    return interlockedPop (rpFirst, pLinkMember, pHazard);
	}
	template <class hazard_pointer_t> bool interlockedPop (BaseClass &rpFirst, LinkMember pLinkMember, hazard_pointer_t &rpHazard) {
	    ReferencedClass *pFirst;
	    do {
		pFirst = m_pReferent;
		rpFirst.setTo (pFirst);
		rpHazard.setTo (pFirst);
	    } while (pFirst && !BaseClass::interlockedSetIf ((pFirst->*pLinkMember).m_pReferent, pFirst));
	    if (!pFirst)
		return false;

	    rpHazard.reset ();
	    V::VAtomicMemoryOperations::MemoryBarrierConsume ();
	    (pFirst->*pLinkMember).clear ();

	    return true;
	}

    //  State
    private:
	using BaseClass::m_pReferent;
    };
}


#endif
