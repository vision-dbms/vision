#ifndef Vca_VTypeInfoHolderFor_Interface
#define Vca_VTypeInfoHolderFor_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VTypeInfoHolder.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    /********************************
     *----  VTypeInfoHolderFor  ----*
     ********************************/

    template <class T> class VTypeInfoHolderFor : public VTypeInfoHolder<T> {
	DECLARE_FAMILY_MEMBERS (VTypeInfoHolderFor<T>, VTypeInfoHolder<T>);

    //  Reference Type
    public:
	typedef typename BaseClass::ReferenceType ReferenceType;

    //  Construction
    public:
	VTypeInfoHolderFor (
	    unsigned int  iD1  , unsigned short  iD2, unsigned short  iD3,
	    unsigned char iD4_0, unsigned char iD4_1, unsigned char iD4_2, unsigned char iD4_3,
	    unsigned char iD4_4, unsigned char iD4_5, unsigned char iD4_6, unsigned char iD4_7
	) : m_iUUID (
	    iD1, iD2, iD3, iD4_0, iD4_1, iD4_2, iD4_3, iD4_4, iD4_5, iD4_6, iD4_7
	) {
// 	    fprintf (stderr, "%8p:%s\n", &BaseClass::g_pThisTypeInfoHolder, rttiName_());
	}

    //  Type Info Instantiation
    protected:
	VTypeInfo *typeInfoInstantiation_() {
	    return VTypeInfo::groundInstance (typeid (T), m_iUUID);
	}

    //  Instance Management
    public:
	void construct_(ReferenceType rT) {
	}
	void preserve_(ReferenceType rT) {
	}
	void destroy_(ReferenceType rT) {
	}

    //  Serialization
    public:
	VcaSerializer *serializer_(VcaSerializer *pCaller, ReferenceType rT) const {
	    return 0;
	}

    //  State
    protected:
	VTypeInfo::UUIDHolder const m_iUUID;
    };
}


#endif
