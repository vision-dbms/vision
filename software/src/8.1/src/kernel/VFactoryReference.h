#ifndef VFactoryReference_Interface
#define VFactoryReference_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReference.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

template <class ReferentType>
class VFactoryReference : public VReference<ReferentType> {
    DECLARE_FAMILY_MEMBERS (VFactoryReference<ReferentType>,VReference<ReferentType>);

//  Initializer and Key Types
public:
    typedef typename ReferentType::Initializer Initializer;
    typedef typename ReferentType::Key Key;

//  Construction
public:
    VFactoryReference () {
    }

//  Destruction
public:
    ~VFactoryReference () {
    }

//  Access
public:
    USING (VReference<ReferentType>::isNil)

//  Comparison (used by VkSetOfFactoryReferences template)
public:
    bool operator < (Key iKey) const {
	return *m_pReferent < iKey;
    }
    bool operator <= (Key iKey) const {
	return *m_pReferent <= iKey;
    }
    bool operator == (Key iKey) const {
	return *m_pReferent == iKey;
    }
    bool operator != (Key iKey) const {
	return *m_pReferent != iKey;
    }
    bool operator >= (Key iKey) const {
	return *m_pReferent >= iKey;
    }
    bool operator > (Key iKey) const {
	return *m_pReferent > iKey;
    }

//  Update
public:
    ThisClass &operator= (ThisClass const &rOther) {
	if (isNil ())
	    BaseClass::setTo (rOther);
	return *this;
    }
    ThisClass &operator= (ReferentType *pReferenceable) {
	if (isNil ())
	    BaseClass::setTo (pReferenceable);
	return *this;
    }
    ThisClass &operator= (Initializer iInitializer) {
	if (isNil ())
	    BaseClass::setTo (new ReferentType (iInitializer));
	return *this;
    }

// State
protected:
    USING (VReference<ReferentType>::m_pReferent)
};


#endif
