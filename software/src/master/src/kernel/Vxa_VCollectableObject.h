#ifndef Vxa_VCollectableObject_Interface
#define Vxa_VCollectableObject_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VRolePlayer.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VCollectableCollectionOf.h"
#include "Vxa_VCollectableMethod.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
    class VAny;
    class VResultBuilder;

    /*-----------------------------------------*
     *----  class Vxa::VCollectableObject  ----*
     *-----------------------------------------*/

    class Vxa_API VCollectableObject : virtual public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VCollectableObject, VRolePlayer);

	friend class VCollectableCollection;

    //  Aliases
    public:
	typedef ThisClass Object;

    //  Class Builder
    public:
	class Vxa_API ClassBuilder;

    //  Traits
    public:
    //  ClusterOf<collectable_reference_t>
    //  ... redefine in derived types to use something other than a VCollectableCollectionOf<T>
        template <typename collectable_reference_t> struct ClusterOf {
            typedef VCollectableCollectionOf<collectable_reference_t> type;
        };

    //  Construction
    protected:
	VCollectableObject ();

    //  Destruction
    protected:
	~VCollectableObject ();

    //  Identity Access
    public:
	cluster_t *objectCluster () const {
	    return m_iIdentity.cluster ();
	}
	cluster_index_t objectIndex () const {
	    return m_iIdentity.clusterIndex ();
	}

    //  Identity Query
    public:
        bool hasAnIdentity () const {
            return m_iIdentity.isSet ();
        }
        bool hasNoIdentity () const {
            return m_iIdentity.isntSet ();
        }

    //  Identity Update
    public:
	bool attach (cluster_t *pCluster, cluster_index_t xObject) {
            return m_iIdentity.attach (pCluster, xObject);
        }
	bool detach (cluster_t *pCluster) {
            return m_iIdentity.detach (pCluster);
        }

    //  State
    private:
        VCollectableIdentity m_iIdentity;
    };

    /*-------------------------------------------------------*
     *----  class Vxa::VCollectableObject::ClassBuilder  ----*
     *-------------------------------------------------------*/

    class Vxa_API VCollectableObject::ClassBuilder {
    //  Construction
    protected:
	ClassBuilder (Vxa::VClass *pClass);

    //  Method Definition
    public:
#ifndef sun
	template <typename T> bool defineConstant (VString const &rName, T rConstant[]) {
	    return defineConstantImpl (rName, static_cast<T*>(rConstant));
	}
#endif
	template <typename T> bool defineConstant (VString const &rName, T const &rConstant) {
	    return defineConstantImpl (rName, rConstant);
	}

	template <typename Signature> bool defineMethod (VString const &rName, Signature pMember) {
	    typename VCollectableMethod<Signature>::Reference pMethod (
		new VCollectableMethod<Signature> (pMember)
	    );
	    return defineMethodImpl (rName, pMethod);
	}

	bool defineHelp (VString const &rWhere);

    private:
	template <typename T> bool defineConstantImpl (VString const &rName, T const &rConstant) {
	    VMethod::Reference pMethod;
	    return VExportable<T>::CreateMethod (pMethod, rConstant) && defineMethodImpl (rName, pMethod);
	}
	bool defineMethodImpl (VString const &rName, VMethod *pMethod);

    //  State
    private:
	VClass* m_pClass;
	VString m_iHelpInfo;
    }; // class Vxa::VCollectableObject

    /*----------------------------------------------------------*
     *----  typedef Vxa::Object => Vxa::VCollectableObject  ----*
     *----------------------------------------------------------*/

    typedef VCollectableObject Object;
}


#endif
