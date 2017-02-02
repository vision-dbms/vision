#ifndef VReference_Interface
#define VReference_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VAtomicMemoryOperations_.h"

namespace V {
    class VRunTimeType;
}

/***********************
 *****  Templates  *****
 ***********************/

template <class ReferenceableClass> class VReference : public VTransient {
//  Family
    DECLARE_FAMILY_MEMBERS (VReference<ReferenceableClass>, VTransient);

//  Aliases
public:
    typedef ReferenceableClass ReferencedClass;

//  Link Member
public:
    typedef ThisClass ReferenceableClass::*LinkMember;

//  Reclamation Control
protected:
    void retainReferent () {
	if (m_pReferent)
	    m_pReferent->retain ();
    }
    void releaseReferent () {
	if (m_pReferent) {
/*============================================================================
 *  'm_pReferent' must be copied and zeroed prior to calling 'release'.
 *  Failure to do so causes problems (e.g., access violations in MSVC Debug
 *  compilations, undefined behavior elsewhere) when the reference being
 *  released breaks a self-reference cycle that results in the immediate
 *  destruction of the object holding the reference.  As originally diagnosed,
 *  the chain of events leading to this problem begins when 'releaseReferent'
 *  is invoked from the 'clear' method.  If the reference being released is
 *  the last reference to the referent, a chain of destructor calls will be
 *  initiated that results in a call to the destructor for the reference being
 *  cleared.  That call will occur before the clear has set the referent to
 *  zero, causing the destructor to erroneously call 'release' again on the
 *  referent which, at this point, has already been destroyed.
 *============================================================================*/
	    ReferenceableClass *pReferent = m_pReferent;
	    m_pReferent = 0;
	    pReferent->release ();
	}
    }

//  Construction
public:
    explicit VReference (ReferenceableClass *pReferent) : m_pReferent (pReferent) {
	retainReferent ();
    }
    VReference (ThisClass const &rOther) : m_pReferent (rOther.m_pReferent) {
	retainReferent ();
    }
    VReference () : m_pReferent (0) {
    }

//  Destruction
public:
    ~VReference () {
	releaseReferent ();
    }

//  Access
public:
    ReferenceableClass* referent () const {
	return m_pReferent;
    }
    ReferenceableClass& operator* () const {
	return *m_pReferent;
    }
    ReferenceableClass* operator-> () const {
	return m_pReferent;
    }
    operator ReferenceableClass* () const {
	return m_pReferent;
    }

//  Query
public:
    bool isEmpty () const {
	return 0 == m_pReferent;
    }
    bool isntEmpty () const {
	return 0 != m_pReferent;
    }

    bool isNil () const {
	return 0 == m_pReferent;
    }
    bool isntNil () const {
	return 0 != m_pReferent;
    }

    bool ownsReferent () const {
	return isntEmpty () && m_pReferent->referenceCount () == 1;
    }
    bool sharesReferent () const {
	return isEmpty () || m_pReferent->referenceCount () > 1;
    }

//  Update
public:
    void claim (ThisClass& rOther) {
	if (this != &rOther) {
	    ReferenceableClass *pReferent = m_pReferent;
	    m_pReferent = rOther.m_pReferent;
	    rOther.m_pReferent = 0;
	    if (pReferent)
		pReferent->release ();
	}
    }

/*===========================================================================
 *  Use the following version of 'claim' with extreme caution, taking
 *  particular care to be sure you actually own the reference you are
 *  claiming.  The original purpose of this routine was to provide a
 *  one statement mechanism for storing references to objects created
 *  with an initial reference count of 1 (the old system convention).
 *  For example, in the following code:
 *
 *	rtLINK_CType *pExpansionLink; M_CPD *pExpansionReordering;
 *	rtLSTORE_Extract (
 *	    rCurrent, pElementCluster, pElementSelector, pExpansionLink, pExpansionReordering
 *	);
 *	m_pExpansionLink.claim (pExpansionLink);
 *	m_pExpansionReordering.claim (pExpansionReordering);
 *
 *  both '*pExpansionLink' and '*pExpansionReordering', when they are
 *  returned, are returned with an initial reference count of 1.  This
 *  is the case 'claim' was written to handle.  The following is an
 *  inappropriate use of claim:
 *
 *	class AClass {
 *	    ComponentClass *component () const {
 *		return m_pComponent;
 *	    }
 *	    VReference<ComponentClass> m_pComponent;
 *	};
 *	    ...
 *	VReference<ComponentClass> pComponentClass;
 *	pComponentClass.claim (pAClass->component());
 *
 *  since the instance of 'AClass', not your code, owns the reference.
 *==========================================================================*/
    void claim (ReferenceableClass *pOther) {
	if (!pOther)
	    releaseReferent ();
	else {
	    pOther->retain ();
	    releaseReferent ();
	    m_pReferent = pOther;
	    pOther->untain ();
	}
    }

    void clear () {
	releaseReferent ();
    }
    bool clearIf (ReferenceableClass *pOld) {
	if (m_pReferent != pOld)
	    return false;
	releaseReferent ();
	return true;
    }

    void push (ReferenceableClass *pFirst, LinkMember pLinkMember) {
	(pFirst->*pLinkMember).setTo (m_pReferent);
	setTo (pFirst);
    }

    bool pop (ThisClass &rpFirst, LinkMember pLinkMember) {
	rpFirst.setTo (m_pReferent);
	if (m_pReferent) {
	    claim (m_pReferent->*pLinkMember);
	    return true;
	}
	return false;
    }

    bool setIf (ReferenceableClass *pNew, ReferenceableClass *pOld) {
	if (m_pReferent != pOld)
	    return false;
	setTo (pNew);
	return true;
    }
    bool setIfNil (ReferenceableClass *pNew) {
	if (m_pReferent)
	    return false;
	setTo (pNew);
	return true;
    }

    void setTo (ReferenceableClass *pOther) {
	if (pOther != m_pReferent) {
	    if (pOther)
		pOther->retain ();

	    releaseReferent ();

	    m_pReferent = pOther;
	}
    }

    ThisClass& operator= (ThisClass const &rOther) {
	setTo (rOther);
	return *this;
    }
    ThisClass& operator= (ReferenceableClass* pOther) {
	setTo (pOther);
	return *this;
    }

//  Interlocked Update
public:
    bool interlockedClearIf (ReferencedClass *pOld) {
	bool bSet = V::VAtomicMemoryOperations_<sizeof (void*)>::interlockedSetIf (
	    (void volatile*)&m_pReferent, pOld, 0
	);
	if (pOld && bSet)
	    pOld->release ();
	return bSet;
    }
    bool interlockedSetIf (ReferenceableClass *pNew, ReferenceableClass *pOld) {
	if (pNew)
	    pNew->retain ();
	bool bSet = V::VAtomicMemoryOperations_<sizeof (void*)>::interlockedSetIf (
	    (void volatile*)&m_pReferent, pOld, pNew
	);
	if (bSet) {
	    if (pOld)
		pOld->release ();
	}
	else if (pNew)
	    pNew->untain ();
	return bSet;
    }
    bool interlockedSetIfNil (ReferenceableClass *pNew) {
	return interlockedSetIf (pNew, 0);
    }

    void interlockedPush (ReferenceableClass *pFirst, LinkMember pLinkMember) {
	V::VAtomicMemoryOperations::MemoryBarrierProduce ();

	ReferenceableClass *&rpFirstLink = (pFirst->*pLinkMember).m_pReferent;
	do {
          (pFirst->*pLinkMember).setTo (m_pReferent);

	} while (!interlockedSetIf (pFirst, rpFirstLink));
    }
    bool interlockedPop (ThisClass &rpFirst, LinkMember pLinkMember) {
	ReferencedClass *pFirst;
	do {
	    pFirst = m_pReferent;
	    rpFirst.setTo (pFirst);
	} while (pFirst && !interlockedSetIf ((pFirst->*pLinkMember).m_pReferent, pFirst));
	if (!pFirst)
	    return false;

	V::VAtomicMemoryOperations::MemoryBarrierConsume ();
	(pFirst->*pLinkMember).clear ();

	return true;
    }

//  Backdoor (intended for use by VcaSerializer's and other reference protective code)
public:
    ReferenceableClass *&_rpReferent_() {
	return m_pReferent;
    }

//  State
protected:
    ReferenceableClass *m_pReferent;
};


#endif
