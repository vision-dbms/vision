#ifndef Vca_VRexecConnectionFactory_Interface
#define Vca_VRexecConnectionFactory_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "V_VBlob.h"

#include "Vca_IBSConsumerClient.h"
#include "Vca_IBSProducerClient.h"
#include "Vca_IPipeSourceClient.h"
#include "Vca_IRexecConnectionFactory.h"

#include "Vca_VPipeSource.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VDeviceFactory.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VRexecConnectionFactory : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VRexecConnectionFactory, VRolePlayer);

    //  Aliases
    public:
	typedef IRexecConnectionFactory ThisInterface;

    //  Factory
    public:
	typedef ThisClass Factory;
	typedef Reference FactoryReference;

    //  Pipe Source
    public:
	class Vca_API PipeSource : public VPipeSource {
	    DECLARE_CONCRETE_RTTLITE (PipeSource, VPipeSource);

	//  Construction
	public:
	    PipeSource (
		Factory *pFactory, 
		VString const &rHost, U32 xPort,
		VString const &rUsername, VString const &rPassword, 
		VString const &rCommand, bool bErrorChannel, bool bNoDelay
	    );

	//  Destruction
	private:
	    ~PipeSource ();

	//  Access/Query
	public:
	    VString const &host () const {
		return m_iHost;
	    }
	    U32 port () const {
		return m_xPort;
	    }
	    VString const &username () const {
		return m_iUsername;
	    }
	    VString const &password () const {
		return m_iPassword;
	    }

	    VString const &command () const {
		return m_iCommand;
	    }
	    Factory *factory () const {
		return m_pFactory;
	    }

	//  Implementation
	private:
	    void supply_(IPipeSourceClient *pClient) OVERRIDE;

	//  State
	private:
	    Factory::Reference	const	m_pFactory;
	    VString		const	m_iHost;
	    U32			const	m_xPort;
	    VString		const	m_iUsername;
	    VString		const	m_iPassword;
	    VString		const	m_iCommand;
	    bool		const	m_bErrorChannel;
	    bool		const	m_bNoDelay;
	};

    //  Transaction
    public:
	class Transaction;

    //  Construction
    private:
	VRexecConnectionFactory (VDeviceFactory *pDeviceFactory);

    //  Destruction
    private:
	~VRexecConnectionFactory ();

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

    //  IRexecConnectionFactory Role
    private:
	VRole<ThisClass,IRexecConnectionFactory> m_pIRexecConnectionFactory;
    public:
	void getRole (IRexecConnectionFactory::Reference &rpRole) {
	    m_pIRexecConnectionFactory.getRole (rpRole);
	}

    //  IRexecConnectionFactory Methods
    public:
	void MakeRexecConnection (
	    IRexecConnectionFactory*	pRole,
	    IPipeSourceClient*		pClient,
	    VString const&		rHost,
	    U32				xPort,
	    VString const&		rUsername,
	    VString const&		rPassword,
	    VString const&		rCommand,
	    bool			bErrorChannel
	);

    //  Local Use
    public:
	static bool Supply (
	    IPipeSourceClient *pClient, 
	    VString const &rHostname, 
	    VString const &rUserName,
	    VString const &rPassword, 
	    VString const &rCommand,
	    bool bErrorChannel,
	    bool bNoDelay,
	    U32 xPort = 512
	);
	bool supply (
	    IPipeSourceClient *pClient, 
	    VString const &rHostname, 
	    VString const &rUserName,
	    VString const &rPassword, 
	    VString const &rCommand,
	    bool bErrorChannel,
	    bool bNoDelay,
	    U32 xPort = 512
	);

	static bool Supply (
	    PipeSource::Reference &rpCPS, 
	    VString const &rHostname, 
	    VString const &rUsername,
	    VString const &rPassword,
	    VString const &rCommand,
	    bool bErrorChannel,
	    bool bNoDelay,
	    U32 xPort = 512
	);
	bool supply (
	    PipeSource::Reference &rpCPS, 
	    VString const &rHostname, 
	    VString const &rUsername,
	    VString const &rPassword,
	    VString const &rCommand,
	    bool bErrorChannel,
	    bool bNoDelay,
	    U32 xPort = 512
	);

    //  State
    protected:
	VDeviceFactory::Reference const m_pDeviceFactory;

    };
}


#endif
