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
    class VTask;

    template <typename Var_T> class VCollectableCollectionOf : public VCollectableCollection {
	DECLARE_CONCRETE_RTTLITE (VCollectableCollectionOf<Var_T>, VCollectableCollection);

    //  Aliases
    public:
	typedef VkDynamicArrayOf<Var_T> container_t;

        typedef typename Var_T::ReferencedClass class_t;
	typedef class_t *val_t;
	typedef Var_T    var_t;

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
    protected:
        virtual VCollectableObject* object_(collection_index_t xElement) const OVERRIDE {
            return element (xElement);
        }
    public:
	val_t operator[] (collection_index_t xElement) const {
	    return element (xElement);
	}
	val_t element (collection_index_t xElement) const {
	    return xElement < cardinality() ? m_iContainer[xElement] : static_cast<val_t>(0);
	}

    //  Task Launcher
    public:
        virtual bool launchTask (VTask *pTask) OVERRIDE {
            return class_t::launchTask (pTask);
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
