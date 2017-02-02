#ifndef V_VPointer_NRK
#define V_VPointer_NRK

/**************************
 *****  Declarations  *****
 **************************/

#include "V_VAtomicMemoryOperations_.h"

#include "V_VFamilyValues.h"

/***********************
 *****  Templates  *****
 ***********************/

namespace V {
    /**
     * Generic pointer that can be used with any datum. Provides basic conveniences over regular pointers. Should not be used directly. For most users, either a subclass or VReference is more useful.
     *
     * @see VReference
     * @see VAggregatePointer
     */
    template <class ReferencedClass> class VPointer {
	DECLARE_FAMILY_MEMBERS (VPointer<ReferencedClass>, void);

    //  Construction
    public:
	explicit VPointer (ReferencedClass *pReferent) : m_pReferent (pReferent) {
	}
	VPointer (ThisClass const &rOther) : m_pReferent (rOther.m_pReferent) {
	}
	VPointer () : m_pReferent (0) {
	}

    //  Destruction
    public:
	~VPointer () {
	}

    //  Access
    public:
	ReferencedClass* referent () const {
	    return m_pReferent;
	}
	ReferencedClass& operator* () const {
	    return *m_pReferent;
	}
	ReferencedClass* operator-> () const {
	    return m_pReferent;
	}
	operator ReferencedClass* () const {
	    return m_pReferent;
	}

    //  Query
    public:
	bool isNil () const {
	    return 0 == m_pReferent;
	}
	bool isntNil () const {
	    return 0 != m_pReferent;
	}

    //  Update
    public:
	void clear () {
	    m_pReferent = 0;
	}
	bool clearIf (ReferencedClass *pOld) {
	    if (m_pReferent != pOld)
		return false;
	    m_pReferent = 0;
	    return true;
	}

	void claim (ThisClass &rOther) {
	    if (this != &rOther) {
		m_pReferent = rOther.m_pReferent;
		rOther.m_pReferent = 0;
	    }
	}

	void setTo (ReferencedClass *pOther) {
	    m_pReferent = pOther;
	}

	bool setIf (ReferencedClass *pNew, ReferencedClass *pOld) {
	    if (m_pReferent != pOld)
		return false;
	    m_pReferent = pNew;
	    return true;
	}
	bool setIfNil (ReferencedClass *pNew) {
	    if (m_pReferent)
		return false;
	    m_pReferent = pNew;
	    return true;
	}

	ThisClass& operator= (ThisClass const &rOther) {
	    setTo (rOther);
	    return *this;
	}
	ThisClass& operator= (ReferencedClass* pOther) {
	    setTo (pOther);
	    return *this;
	}

    //  Interlocked Update
    public:
	bool interlockedClearIf (ReferencedClass *pOld) {
	    return VAtomicMemoryOperations_<sizeof (void*)>::interlockedSetIf (
		(void volatile*)&m_pReferent, pOld, 0
	    );
	}
	bool interlockedSetIf (ReferencedClass *pNew, ReferencedClass *pOld) {
	    return VAtomicMemoryOperations_<sizeof (void*)>::interlockedSetIf (
		(void volatile*)&m_pReferent, pOld, pNew
	    );
	}
	bool interlockedSetIfNil (ReferencedClass *pNew) {
	    return interlockedSetIf (pNew, 0);
	}

    //  State
    protected:
	ReferencedClass *m_pReferent;
    };
}


/***************************
 *****  Pointer Types  *****
 ***************************/

#include "V_VAggregatePointer_NRK.h"


#endif
