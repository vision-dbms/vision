#ifndef Vca_VConnectionFactory_Interface
#define Vca_VConnectionFactory_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IConnectionFactory.h"
#include "Vca_IPipeFactory.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VDeviceFactory.h"


/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VConnection;
    class VConnectionPipeSource;

    class Vca_API VConnectionFactory : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VConnectionFactory, VRolePlayer);

    //  Aliases
    public:
	typedef IVReceiver<IConnection*> IConnectionSink;

    //  Construction
    private:
	VConnectionFactory (VDeviceFactory *pDeviceFactory);

    //  Destruction
    private:
	~VConnectionFactory ();

    //  Cohort Index
    private:
	static VCohortIndex const &CohortIndex ();

    //  Cohort Instance
    public:
	static bool Supply (Reference &rpInstance);
	static bool Supply (IConnectionFactory::Reference &rpRole);
	static bool Supply (IPipeFactory::Reference &rpRole);

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  IConnectionFactory Role
    private:
	VRole<ThisClass,IConnectionFactory> m_pIConnectionFactory;
    public:
	void getRole (IConnectionFactory::Reference &rpRole) {
	    m_pIConnectionFactory.getRole (rpRole);
	}

    //  IConnectionFactory Methods
    public:
	void MakeConnection (
	    IConnectionFactory *pRole, IConnectionSink *pClient, VString const &rName
	);

    //  IPipeFactory Role
    private:
	VRole<ThisClass,IPipeFactory> m_pIPipeFactory;
    public:
	void getRole (IPipeFactory::Reference &rpRole) {
	    m_pIPipeFactory.getRole (rpRole);
	}

    //  IPipeFactory Methods
    public:
	void MakeConnection (
	    IPipeFactory *pRole, IPipeSourceClient *pClient, VString const &rName, bool bTwoPipe
	);

    //  Local Use
    public:
	static bool Supply (IConnectionSink *pClient, char const *pName, bool bNoDelay);
	bool supply (IConnectionSink *pClient, char const *pName, bool bNoDelay);

	static bool Supply (IPipeSourceClient *pClient, char const *pName, bool bNoDelay);
	bool supply (IPipeSourceClient *pClient, char const *pName, bool bNoDelay);

	static bool Supply (VReference<VConnectionPipeSource>&rpCPS, VString const &rDestination, bool bIsFileName, bool bNoDelay);
	bool supply (VReference<VConnectionPipeSource>&rpCPS, VString const &rDestination, bool bIsFileName, bool bNoDelay);

    //  State
    protected:
	VDeviceFactory::Reference const m_pDeviceFactory;
    };
}            


#endif
