#ifndef Vxa_VCollectableCollection_Interface
#define Vxa_VCollectableCollection_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VCollection.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VClass.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VCollectableObject;

    class Vxa_API VCollectableCollection : public VCollection {
	DECLARE_ABSTRACT_RTTLITE (VCollectableCollection, VCollection);

    //  Aliases
    public:
	typedef cardinality_t collection_index_t;

    //  Construction
    protected:
	VCollectableCollection (VClass *pClass);

    //  Destruction
    protected:
	~VCollectableCollection ();

    //  Access
    private:
	virtual cardinality_t cardinality_() const {
	    return cardinality ();
	}
    public:
	cardinality_t cardinality () const {
	    return m_sCollection;
	}
	VClass *type () const {
	    return m_pClass;
	}

    //  Accounting
    protected:
	void onCollectionCreation (cardinality_t sCollection) const {
	    m_pClass->onCollectionCreation (sCollection);
	}
	void onCollectionDeletion (cardinality_t sCollection) const {
	    m_pClass->onCollectionDeletion (sCollection);
	}

    //  Method Invocation
    private: //  Override of VCollection::invokeMethod_ ...
	virtual bool invokeMethod_(VString const &rMethodName, VCallHandle const &rCallHandle) {
	    return invokeMethod (rMethodName, rCallHandle);
	}
    protected:
	bool invokeMethod (VString const &rMethodName, VCallHandle const &rCallHandle) {
	    return m_pClass->invokeMethod (rMethodName, rCallHandle, this);
	}

    //  Transmission
    protected:
	bool transmitUsing_(VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker, object_reference_array_t const &rInjection);
	bool transmitUsing_(VCallType2Exporter *pExporter, VMonotypeMapMaker *pMapMaker);

    //  Update
    protected:
	bool attach (VCollectableObject *pObject);
	bool detach (VCollectableObject *pObject);

    //  State
    private:
	VClass::Pointer const m_pClass;
	cardinality_t m_sCollection;
    };
}


#endif
