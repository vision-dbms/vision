#ifndef VInstanceHolder_Interface
#define VInstanceHolder_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VTypeInfoHolder.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VcaSerializer;

    template <class T> class VInstanceHolder : public virtual VTransient {
    //  Family
	DECLARE_FAMILY_MEMBERS (VInstanceHolder<T>, VTransient);

    //  StorageType
    public:
	typedef typename VTypeTraits<T>::StorageType StorageType;

    //  StorageTypeInfoHolder
    public:
	typedef VTypeInfoHolder<StorageType> StorageTypeInfoHolder;

    //  Construction
    public:
	VInstanceHolder (T iT) : m_iT (iT) {
	    StorageTypeInfoHolder::preserve (m_iT);
	}
	VInstanceHolder () {
	    StorageTypeInfoHolder::construct (m_iT);
	}

    //  Destruction
    public:
	~VInstanceHolder () {
	    StorageTypeInfoHolder::destroy (m_iT);
	}

    //  Assignment (Disabled)
    public:
	void setTo (T iT) {
	    StorageTypeInfoHolder::preserve (iT);
	    StorageTypeInfoHolder::destroy (m_iT);
	    m_iT = iT;
	}
	ThisClass &operator= (T iT) {
	    setTo (iT);
	    return *this;
	}

    //  Access
    public:
	operator StorageType const& () const {
	    return m_iT;
	}
	operator StorageType& () {
	    return m_iT;
	}

    //  Serialization
    public:
	VcaSerializer *serializer (VcaSerializer *pCaller) {
	    return StorageTypeInfoHolder::serializer (pCaller, m_iT);
	}

    //  State
    protected:
	StorageType m_iT;
    };
}


#endif
