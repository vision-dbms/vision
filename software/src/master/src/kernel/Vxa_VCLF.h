#ifndef Vxa_VCLF_Interface
#define Vxa_VCLF_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vxa_VRolePlayer.h"

#include "Vxa_ISingleton.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vxa_VCollection.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {
/*******************************************
 *----  VCLF == V Client List Factory  ----*
 *******************************************
 *
 *  Once upon a time, the original implementation of the Vision adapter supported a
 *  clunky mechanism that batchvision could use to create adapter side collections.
 *  That mechanism isn't needed for the Bridge and has been removed except for
 *  the artifact of the VCLF stub that plays role of the client-list-factory used
 *  by Vision to create these adapter side collections.
 *
 ********************************************/
    class VCLF : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VCLF, VRolePlayer);

    //  Construction
    public:
	VCLF (VCollection *pCluster);

    //  Destruction
    private:
	~VCLF ();

    //  Roles
    public:
	using BaseClass::getRole;

    //  ISingleton
    private:
	Vca::VRole<ThisClass,ISingleton> m_pISingleton;
    public:
	void getRole (ISingleton::Reference &rpRole) {
	    m_pISingleton.getRole (rpRole);
	}

    //  ISingleton Methods
    public:
	void ExternalImplementation (
	    ISingleton *pRole, IVSNFTaskHolder *pCaller, VString const &rMessageName, cardinality_t cParameters, cardinality_t cTask
	);

    //  State
    private:
        VCollection::Reference const m_pCluster;
    };
}


#endif
