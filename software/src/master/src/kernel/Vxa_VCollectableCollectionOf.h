#ifndef Vxa_VCollectableCollectionOf_Interface
#define Vxa_VCollectableCollectionOf_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCollectableCollection.h"

#include "VkDynamicArrayOf.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VCardinalityHints.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    template <typename Var_T> class VCollectableCollectionOf : public VCollectableCollection {
	DECLARE_CONCRETE_RTTLITE (VCollectableCollectionOf<Var_T>, VCollectableCollection);

    //  Aliases
    public:
	typedef VkDynamicArrayOf<Var_T> container_t;

	typedef typename Var_T::ReferencedClass *val_t;
	typedef Var_T var_t;

    //  Construction
    public:
	VCollectableCollectionOf (
	    VClass *pClass, VCardinalityHints *pTailHints, val_t pInstance
	) : BaseClass (pClass), m_iContainer (pTailHints ? pTailHints->span () : 1) {
//################################################################
//  Calling attach causes a nasty stack dump which is very
//  useful for testing error detection and recovery...
//	    attach (pInstance);
	    append (pInstance);
	    onCollectionCreation (1);
	}

    //  Destruction
    protected:
	~VCollectableCollectionOf () {
	    for (collection_index_t xObject = 0; xObject < m_iContainer.cardinality (); xObject++)
		BaseClass::detach (m_iContainer[xObject]);
	    onCollectionDeletion (cardinality ());
	}

    //  Access
    public:
	val_t operator[] (unsigned int xElement) const {
	    return element (xElement);
	}
	val_t element (unsigned int xElement) const {
	    return xElement < cardinality() ? m_iContainer[xElement] : static_cast<val_t>(0);
	}

    //  Update
    public:
	void append (val_t pInstance) {
	    collection_index_t const xObject = cardinality ();
	    if (xObject >= m_iContainer.cardinality ())
		m_iContainer.Append (xObject - m_iContainer.cardinality () + 1);
	    m_iContainer[xObject].setTo (pInstance);
	    attach (pInstance);
	}

    //  State
    private:
	container_t m_iContainer;
    };
}


#endif
