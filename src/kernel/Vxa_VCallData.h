#ifndef Vxa_VCallData_Interface
#define Vxa_VCallData_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VRolePlayer.h"

#include "Vxa_VFiniteSet.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VCollection.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class Vxa_API VCallData {
	DECLARE_NUCLEAR_FAMILY (VCallData);

    //  Selector
    public:
        class Vxa_API Selector {
        //  Construction
        public:
            Selector (VString const &rName, cardinality_t cParameters);
            Selector (Selector const &rOther);

        //  Destruction
        public:
            ~Selector ();

        //  Access
        public:
            VString const &fullName () const {
                return m_iName;
            }

            VString const &component (cardinality_t xComponent) const;
            bool component (VString &rComponent, cardinality_t xComponent) const;

            cardinality_t parameterCount () const {
                return m_aComponents.elementCount ();
            }

        //  State
        private:
            VString                   const m_iName;
            VkDynamicArrayOf<VString> const m_aComponents;
        };

    //  Construction
    public:
	VCallData (
            VCollection *pCluster, VString const &rSelectorName, cardinality_t cParameters, cardinality_t cTask, bool bIntensional
        );
	VCallData (ThisClass const &rOther);

    //  Destruction
    public:
	~VCallData ();

    //  Access
    public:
	cardinality_t cardinality () const {
	    return m_pDomain->cardinality ();
	}
        VCollection *cluster() const {
            return m_pCluster;
        }
        VCollectableObject *clusterObject (object_reference_t xObject) const {
            return m_pCluster->object (xObject);
        }
        VClass *clusterType () const {
            return m_pCluster->type ();
        }
	VFiniteSet *domain () const {
	    return m_pDomain;
	}
        bool invokedIntensionally () const {
            return m_bIntensional;
        }
	cardinality_t parameterCount () const {
	    return m_iSelector.parameterCount ();
	}
        VString const &selectorName () const {
            return m_iSelector.fullName ();
        }
        VString const &selectorComponent (cardinality_t xComponent) const {
            return m_iSelector.component (xComponent);
        }
        bool selectorComponent (VString &rComponent, cardinality_t xComponent) const {
            return m_iSelector.component (rComponent, xComponent);
        }

    //  Task Management
    public:
        bool launchTask (VTask *pTask) const;

    //  State
    private:
        VCollection::Reference const m_pCluster;
        Selector               const m_iSelector;
	VFiniteSet::Reference  const m_pDomain;
        bool                   const m_bIntensional;
    };
}


#endif
