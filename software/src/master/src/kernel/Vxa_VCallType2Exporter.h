#ifndef Vxa_VCallType2Exporter_Interface
#define Vxa_VCallType2Exporter_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VResultBuilder.h"
#include "Vxa_VCallType2.h"

#include "Vxa_VMapMaker.h"
#include "Vxa_VTask.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VCollectableCollectionOf.h"
#include "Vxa_VMonotypeMapMakerFor.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VCallType2Exporter : public VResultBuilder, public VCallType2 {
	DECLARE_FAMILY_MEMBERS (VCallType2Exporter, VResultBuilder);

    //  Aliases
    public:
	typedef cardinality_t object_index_t;
	typedef VMonotypeMapMakerFor<object_index_t> map_maker_t;

	typedef VkMapOf<VClass::Pointer,VClass*,VClass const*,VCollectableCollection::Reference> cluster_map_t;

    //  Construction
    public:
	VCallType2Exporter (VTask *pTask, VCallType2Importer const &rCallImporter);

    //  Destruction
    public:
	~VCallType2Exporter ();

    //  Element Update
    private:
	virtual bool returnResult (VExportableDatum const &rDatum);
    public:
	template <typename object_reference_t> bool returnObject (VClass *pClass, object_reference_t const &rpObject) {
	    VCollectableCollection::Reference pObjectCluster; object_index_t xObject;
	    getObjectIdentity (rpObject, pClass, pObjectCluster, xObject);

	    map_maker_t::Reference pMapMaker;
	    updateMap (pMapMaker, pObjectCluster, xObject);

	    return true;
	}
	template <typename map_maker_reference_t, typename value_t> bool returnResult (
	    map_maker_reference_t &rpMapMaker, VExportableType *pType, VSet *pCodomain, value_t iValue
	) {
	    updateMap (rpMapMaker, pCodomain, iValue);
	    return true;
	}

    //  Object Management
    private:
	template <typename object_reference_t> void getObjectIdentity (
	    object_reference_t const &rpObject, VClass *pClass, VCollectableCollection::Reference &rpObjectCluster, object_index_t &rxObject
	) {
            typedef typename object_reference_t::ReferencedClass::template ClusterOf<object_reference_t>::type cluster_t;
	    if (rpObject->getIdentity (rpObjectCluster, rxObject))
		return;

	    unsigned int xObjectCluster;
	    if (m_iClusterMap.Insert (pClass,xObjectCluster))
	    //  Cluster doesn't exist, create it...
		m_iClusterMap[xObjectCluster].setTo (new cluster_t (pClass, tailHints (), rpObject.referent ()));
	    else {
	    //  Cluster exists, add object to it...
		typename cluster_t::Reference const pCluster (
		    static_cast<cluster_t*>(m_iClusterMap[xObjectCluster].referent ())
		);
		pCluster->append (rpObject);
	    }
	    rpObject->getIdentity (rpObjectCluster, rxObject);
	}

    //  Map Making
    private:
	/**********************************************************************************
	 *  'getMapMakerFor' returns the map maker associated with 'pDomain'.  If this is
	 *  is the first occurrence of this codomain in the result, the map maker will be
	 *  'unimplemented' and the caller is responsible for creating an implementation.
	 **********************************************************************************/
	VMapMaker &getMapMakerFor (VSet *pCodomain);

	template <typename map_maker_reference_t, typename value_t> void updateMap (
	    map_maker_reference_t &rpMapMaker, VSet *pCodomain, value_t iValue
	) {
	    VMapMaker &rMapMaker (getMapMakerFor (pCodomain));
	    if (rMapMaker.getImplementation (rpMapMaker))
		rpMapMaker->setCurrentValueTo (iValue);
	    else {
		rpMapMaker.setTo (
		    new typename map_maker_reference_t::ReferencedClass (tailHints (), pCodomain, iValue)
		);
		rMapMaker.setImplementationTo (rpMapMaker);
	    }
	}

    //  State
    private:
	cluster_map_t m_iClusterMap;
	VMapMaker     m_iMapMaker;
    };
}


#endif
