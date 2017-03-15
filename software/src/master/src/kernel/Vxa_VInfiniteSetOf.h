#ifndef Vxa_VInfiniteSetOf_Interface
#define Vxa_VInfiniteSetOf_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VInfiniteSet.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    template <class Type_T> class VInfiniteSetOf : public VInfiniteSet {
	DECLARE_CONCRETE_RTTLITE (VInfiniteSetOf<Type_T>, VInfiniteSet);

    //  Construction
    public:
	VInfiniteSetOf (Type_T *pType) : m_pType (pType) {
	}

    //  Destruction
    protected:
	~VInfiniteSetOf () {
	}

    //  Access
    public:
	Type_T *type () const {
	    return m_pType;
	}

    //  Description
    protected:
	VString &getDescription_(VString &rResult) const {
	    rResult << "SetOf<";
	    return m_pType->getDescription (rResult) << ">";
	}

    //  State
    private:
	typename Type_T::Pointer const m_pType;
    };
}


#endif
