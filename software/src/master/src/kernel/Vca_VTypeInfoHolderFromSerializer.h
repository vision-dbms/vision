#ifndef Vca_VTypeInfoHolderFromSerializer_Interface
#define Vca_VTypeInfoHolderFromSerializer_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VTypeInfoHolderForSerializable.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template <class Serializer_T>
	class VTypeInfoHolderFromSerializer : public VTypeInfoHolderForSerializable<typename Serializer_T::DataType, Serializer_T>
    {
    //  Family Values
	typedef VTypeInfoHolderForSerializable<typename Serializer_T::DataType, Serializer_T> base_t;
	DECLARE_FAMILY_MEMBERS (Serializer_T, base_t);

    //  Construction
    public:
	VTypeInfoHolderFromSerializer (
	    unsigned int  iD1  , unsigned short  iD2, unsigned short  iD3,
	    unsigned char iD4_0, unsigned char iD4_1, unsigned char iD4_2, unsigned char iD4_3,
	    unsigned char iD4_4, unsigned char iD4_5, unsigned char iD4_6, unsigned char iD4_7
	) : BaseClass (
	    iD1, iD2, iD3, iD4_0, iD4_1, iD4_2, iD4_3, iD4_4, iD4_5, iD4_6, iD4_7
	) {
	}
    };
}


#endif
