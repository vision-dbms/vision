#ifndef V_VAggregatePointer_NRK
#define V_VAggregatePointer_NRK

/***********************
 *****  Templates  *****
 ***********************/

namespace V {
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
    template <class ReferencedClass> class VAggregatePointer : public VPointer<ReferencedClass> {
    //  Family
	DECLARE_FAMILY_MEMBERS (VAggregatePointer<ReferencedClass>, VPointer<ReferencedClass>);

    //  Link Member
    public:
	typedef ThisClass ReferencedClass::*LinkMember;

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
         * Thread-safe stack push operation.
         */
	void interlockedPush (ReferencedClass *pFirst, LinkMember pLinkMember) {
	    VAtomicMemoryOperations::MemoryBarrierProduce ();

	    ReferencedClass *&rpFirstLink = (pFirst->*pLinkMember).m_pReferent;
	    do {
		rpFirstLink = m_pReferent;
	    } while (!interlockedSetIf (pFirst, rpFirstLink));
	}
        /**
         * Thread-safe stack pop operation.
         * 
         * @todo solve ABA vulnerability.
         */
	bool interlockedPop (ReferencedClass *&rpFirst, LinkMember pLinkMember) {
	    ReferencedClass *pFirst;
	    do {
		pFirst = m_pReferent;
	    } while (pFirst && !interlockedSetIf ((pFirst->*pLinkMember).m_pReferent, pFirst));
	    rpFirst = pFirst;
	    if (!pFirst)
		return false;

	    VAtomicMemoryOperations::MemoryBarrierConsume ();
	    (pFirst->*pLinkMember).m_pReferent = 0;

	    return true;
	}
	bool interlockedPop (ThisClass &rpFirst, LinkMember pLinkMember) {
	    return interlockedPop (rpFirst.m_pReferent, pLinkMember);
	}

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
}


#endif
