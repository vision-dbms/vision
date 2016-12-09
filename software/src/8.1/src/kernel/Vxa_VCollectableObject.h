#ifndef Vxa_VCollectableObject_Interface
#define Vxa_VCollectableObject_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VRolePlayer.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VCollectableCollection.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VCollectableObject : virtual public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VCollectableObject, VRolePlayer);

	friend class VCollectableCollection;

    //  Aliases
    public:
	typedef VCollectableCollection::collection_index_t collection_index_t;

    //  Construction
    protected:
	VCollectableObject ();

    //  Destruction
    protected:
	~VCollectableObject ();

    //  Access
    public:
	VCollectableCollection *objectCollection () const {
	    return m_pCollection;
	}
	collection_index_t objectIndex () const {
	    return m_xObject;
	}

    //  Identity Access
    public:
	bool getIdentity (VCollectableCollection::Reference &rpCollection, collection_index_t &rxObject) const {
	    rpCollection = m_pCollection;
	    rxObject = m_xObject;
	    return rpCollection.isntNil ();
	}

    //  Identity Management
    public:
	bool attach (VCollectableCollection *pCollection, collection_index_t xObject);
	bool detach (VCollectableCollection *pCollection);

    //  State
    private:
	VCollectableCollection::Pointer m_pCollection;
	collection_index_t m_xObject;
    };
}


#endif
