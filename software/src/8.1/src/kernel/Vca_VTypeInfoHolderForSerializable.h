#ifndef Vca_VTypeInfoHolderForSerializable_Interface
#define Vca_VTypeInfoHolderForSerializable_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VTypeInfoHolderFor.h"

#include "Vca_VcaSerializer_.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template <typename T, class Serializer_T = VcaSerializer_<T> >
    class VTypeInfoHolderForSerializable : public VTypeInfoHolderFor<T> {
    //  Family Values
	typedef VTypeInfoHolderForSerializable<T,Serializer_T> this_t;
	DECLARE_FAMILY_MEMBERS (this_t, VTypeInfoHolderFor<T>);

    //  Reference Type
    public:
	typedef typename BaseClass::ReferenceType ReferenceType;

    //  Construction
    public:
	VTypeInfoHolderForSerializable (
	    unsigned int  iD1  , unsigned short  iD2, unsigned short  iD3,
	    unsigned char iD4_0, unsigned char iD4_1, unsigned char iD4_2, unsigned char iD4_3,
	    unsigned char iD4_4, unsigned char iD4_5, unsigned char iD4_6, unsigned char iD4_7
	) : BaseClass (
	    iD1, iD2, iD3, iD4_0, iD4_1, iD4_2, iD4_3, iD4_4, iD4_5, iD4_6, iD4_7
	) {
	}

    //  Serialization
    public:
	VcaSerializer *serializer_(VcaSerializer *pCaller, ReferenceType rT) const {
	    return new Serializer_T (pCaller, rT);
	}
    };
}


#endif
