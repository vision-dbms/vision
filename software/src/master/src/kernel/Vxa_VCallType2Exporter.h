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
	typedef VMonotypeMapMakerFor<cluster_index_t> map_maker_t;

	typedef VkMapOf<VClass::Pointer,VClass*,VClass const*,VCollectableCollection::Reference> cluster_map_t;

    //  Construction
    public:
	VCallType2Exporter (VTask *pTask, VCallType2Importer const &rCallImporter);

    //  Destruction
    public:
	~VCallType2Exporter ();

    //  Element Update
    private:
	virtual bool returnResult (VExportableDatum const &rDatum) OVERRIDE;
    public:
	template <typename object_reference_t> bool returnObject (VClass *pClass, object_reference_t const &rpObject) {
	    getObjectIdentity (rpObject, pClass);

	    map_maker_t::Reference pMapMaker;
	    updateMap (pMapMaker, rpObject->objectCluster (), rpObject->objectIndex ());

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
	    object_reference_t const &rpObject, VClass *pClass
	) {
        //  If the object no identity ...
	    if (rpObject->hasNoIdentity ()) {
            // ... access the cluster map slot associated with the object's type:
                unsigned int xCluster;
                m_iClusterMap.Insert (pClass,xCluster);

            //  ... and assign an identity based on the cluster associated with that slot:
                VCollectableTraits<object_reference_t>::CreateIdentity (
                    m_iClusterMap[xCluster], rpObject, pClass, tailHints ()
                );
            }
	}

    //  Remote Control
    private:
        virtual Vxa::ICaller2 *getRemoteControlInterface () const override {
            return VCallType2::getRemoteControlInterface ();
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
