#ifndef Vca_VListenerFactory_Interface
#define Vca_VListenerFactory_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IListenerFactory.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VDeviceFactory.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VListenerFactory : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VListenerFactory, VRolePlayer);

    //  Aliases
    public:
	typedef IListenerFactory ThisInterface;

    //  Construction
    private:
	VListenerFactory (VDeviceFactory *pDeviceFactory);

    //  Destruction
    private:
	~VListenerFactory ();

    //  Cohort Index
    private:
	static VCohortIndex const &CohortIndex ();

    //  Cohort Instance
    public:
	static bool Supply (Reference &rpInstance);
	static bool Supply (ThisInterface::Reference &rpRole);

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  IListenerFactory Role
    private:
	VRole<ThisClass,IListenerFactory> m_pIListenerFactory;
    public:
	void getRole (IListenerFactory::Reference &rpRole) {
	    m_pIListenerFactory.getRole (rpRole);
	}

    //  IListenerFactory Methods
    public:
	void SupplyListener (
	    IListenerFactory *pRole, IVReceiver<IListener*> *pClient, VString const &rName
	);
	void SupplyByteStreams (
	    IListenerFactory*	pRole,
	    IPipeSourceClient*	pClient,
	    VString const&	rDestination,
	    U32			cConnections
	);
	void SupplyConnections (
	    IListenerFactory*	pRole,
	    IListenerClient*	pClient,
	    VString const&	rDestination,
	    U32			cConnections
	);

    //  State
    private:
	VDeviceFactory::Reference const m_pDeviceFactory;
    };
}


#endif
