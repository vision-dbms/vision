#ifndef Vca_VTypeInfoHolderForReferenceable_Interface
#define Vca_VTypeInfoHolderForReferenceable_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VTypeInfoHolderForSerializable.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template <class T>
    class VTypeInfoHolderForReferenceable : public VTypeInfoHolderForSerializable<T*> {
    //  Family Values
	DECLARE_FAMILY_MEMBERS (
	    VTypeInfoHolderForReferenceable<T>, VTypeInfoHolderForSerializable<T*>
	);

    //  Reference Type
    public:
	typedef typename BaseClass::ReferenceType ReferenceType;

    //  Construction
    public:
	VTypeInfoHolderForReferenceable (
	    unsigned int  iD1  , unsigned short  iD2, unsigned short  iD3,
	    unsigned char iD4_0, unsigned char iD4_1, unsigned char iD4_2, unsigned char iD4_3,
	    unsigned char iD4_4, unsigned char iD4_5, unsigned char iD4_6, unsigned char iD4_7
	) : BaseClass (
	    iD1, iD2, iD3, iD4_0, iD4_1, iD4_2, iD4_3, iD4_4, iD4_5, iD4_6, iD4_7
	) {
	}

    //  Instance Management
    protected:
	void construct_(ReferenceType rT) {
	    rT = 0;
	}
	void preserve_(ReferenceType rT) {
	    if (rT)
		rT->retain ();
	}
	void destroy_(ReferenceType rT) {
	    if (rT)
		rT->release ();
	}
    };
}


#endif
