#ifndef V_VAggregatePointer_NRK
#define V_VAggregatePointer_NRK

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VAtomicLink.h"

/***********************
 *****  Templates  *****
 ***********************/

namespace V {
    /*******************************************************************
     *----  template <class Referenced_T> class VAggregatePointer  ----*
     *******************************************************************/

    /**
     * VPointer subclassed enhanced with stack operations.
     * VAggregatePointer instances can be treated like a stack using their push() and pop() methods (or interlockedPush() and interlockedPop() for thread safety).
     * 
     * @todo Document requisites of stack usage with VAggregatePointer.
     * 
     * @see push()
     * @see pop()
     * @see interolockedPush()
     * @see interlockedPop()
     */
    template <class Referenced_T> class VAggregatePointer : public VPointer<Referenced_T> {
	DECLARE_FAMILY_MEMBERS (VAggregatePointer<Referenced_T>, VPointer<Referenced_T>);

	friend class VAtomicLink<ThisClass>;

    //  Aliases
    public:
	typedef Referenced_T ReferencedClass;

    //  Link Member
    public:
	typedef ThisClass Referenced_T::*LinkMember;

    //  Construction
    public:
	explicit VAggregatePointer (ReferencedClass *pReferent) : BaseClass (pReferent) {
	}
	VAggregatePointer (ThisClass const &rOther) : BaseClass (rOther) {
	}
	VAggregatePointer () {
	}

    //  Destruction
    public:
	~VAggregatePointer () {
	}

    //  Update
    public:
	USING (BaseClass::setTo)
	ThisClass &operator= (ReferencedClass *pOther) {
	    setTo (pOther);
	    return *this;
	}
	ThisClass &operator= (ThisClass const &rOther) {
	    setTo (rOther);
	    return *this;
	}

    //  Interlocked Update
    public:
	USING (BaseClass::interlockedSetIf)

    public:
        /**
         * Thread-safe stack push operation:
	 *
	 *    void interlockedPush (ReferencedClass *pFirst, LinkMember pLinkMember);
	 *
	 *  moved to V::VAtomicallyLinkablePointer.
         */

        /**
         * Thread-safe stack pop operation:
         *
	 *    bool interlockedPop (ReferencedClass *&rpFirst, LinkMember pLinkMember);
	 *
	 *  moved to V::VAtomicallyLinkablePointer.
         */

    //  Non-Interlocked Update
    public:
	USING (BaseClass::claim)
	USING (BaseClass::setIf)
	USING (BaseClass::setIfNil)

	void push (ReferencedClass *pFirst, LinkMember pLinkMember) {
	    (pFirst->*pLinkMember).m_pReferent = m_pReferent;
	    m_pReferent = pFirst;
	}

	bool pop (ReferencedClass *&rpFirst, LinkMember pLinkMember) {
	    rpFirst = m_pReferent;
	    if (rpFirst) {
		claim (m_pReferent->*pLinkMember);
		return true;
	    }
	    return false;
	}
	bool pop (ThisClass &rpFirst, LinkMember pLinkMember) {
	    return pop (rpFirst.m_pReferent, pLinkMember);
	}

    //  State
    protected:
	USING (BaseClass::m_pReferent)
    };


    /****************************************************************************
     *----  template <class Referenced_T> class VAtomicallyLinkablePointer  ----*
     ****************************************************************************/

    template <class Referenced_T> class VAtomicallyLinkablePointer : public VAtomicLink<VAggregatePointer<Referenced_T> > {
	DECLARE_FAMILY_MEMBERS (VAtomicallyLinkablePointer<Referenced_T>,VAtomicLink<VAggregatePointer<Referenced_T> >);

    //  Construction
    public:
	explicit VAtomicallyLinkablePointer (Referenced_T *pReferent) : BaseClass (pReferent) {
	}
	VAtomicallyLinkablePointer (ThisClass const &rOther) : BaseClass (rOther) {
	}
	VAtomicallyLinkablePointer () {
	}

    //  Destruction
    public:
	~VAtomicallyLinkablePointer () {
	}
    };
}


#endif
