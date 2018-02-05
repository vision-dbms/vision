#ifndef Vca_VProxyApp_Interface
#define Vca_VProxyApp_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VServerApplication.h"

#include "Vca_IConnectionFactory.h"
#include "Vca_IListenerFactory.h"
#include "Vca_IPipeFactory.h"
#include "Vca_IProcessFactory.h"
#include "Vca_IStdPipeSource.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VProxyApp : public VServerApplication {
	DECLARE_CONCRETE_RTTLITE (VProxyApp, VServerApplication);

    //  Construction
    public:
	VProxyApp (Context *pContext);

    //  Destruction
    private:
	~VProxyApp () {
	}

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
    public/*private*/:
	void MakeConnection (
	    IConnectionFactory *pRole, IVReceiver<IConnection*> *pClient,
	    VString const &rDestination
	);

    //  IListenerFactory Role
    private:
	VRole<ThisClass,IListenerFactory> m_pIListenerFactory;
    public:
	void getRole (IListenerFactory::Reference &rpRole) {
	    m_pIListenerFactory.getRole (rpRole);
	}

    //  IListenerFactory Methods
    public/*private*/:
	void SupplyByteStreams (
	    IListenerFactory *pRole, IPipeSourceClient *pClient,
	    VString const &rName, U32 cConnections
	);
	void SupplyConnections (
	    IListenerFactory *pRole, IListenerClient *pClient,
	    VString const &rName, U32 cConnections
	);
	void SupplyListener (
	    IListenerFactory *pRole, IVReceiver<IListener*>*pClient, VString const &rName
	);

    //  IPipeFactory Role
    private:
	VRole<ThisClass,IPipeFactory> m_pIPipeFactory;
    public:
	void getRole (IPipeFactory::Reference &rpRole) {
	    m_pIPipeFactory.getRole (rpRole);
	}

    //  IPipeFactory Methods
    public/*private*/:
	void MakeConnection (
	    IPipeFactory *pRole, IPipeSourceClient *pClient,
	    VString const &rDestination, bool bTwoPipe
	);

    //  IProcessFactory Role
    private:
	VRole<ThisClass,IProcessFactory> m_pIProcessFactory;
    public:
	void getRole (IProcessFactory::Reference &rpRole) {
	    m_pIProcessFactory.getRole (rpRole);
	}

    //  IProcessFactory Methods
    public/*private*/:
	void MakeProcess (
	    IProcessFactory *pRole, IProcessFactoryClient *pClient,
	    VString const &rCommand, bool bTwoPipe
	);

    //  IStdPipeSource Role
    private:
	VRole<ThisClass,IStdPipeSource> m_pIStdPipeSource;
    public:
	void getRole (IStdPipeSource::Reference &rpRole) {
	    m_pIStdPipeSource.getRole (rpRole);
	}

    //  IStdPipeSource Methods
    public/*private*/:
	void Supply (
	    IStdPipeSource *pRole, IStdPipeSourceClient *pClient
	);

    //  Run State Transition
    private:
	bool start_() OVERRIDE;
    };
}


#endif
