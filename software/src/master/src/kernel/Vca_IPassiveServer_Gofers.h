#ifndef Vca_IPassiveServer_Gofers_Interface
#define Vca_IPassiveServer_Gofers_Interface

#ifndef Vca_IPassiveServer_Gofers
#define Vca_IPassiveServer_Gofers extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInterface.h"

#include "Vca_IPassiveServer.h"

#include "Vca_VInstanceOfInterface.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    DECLARE_GOFER_INTERFACE (Vca_IPassiveServer_Gofers, Vca_API, IPassiveServer)

    namespace IPassiveServer_Gofer {
	typedef VGoferInterface<IPassiveServer> gofer_base_t;

	class Vca_API LaunchAgent : public gofer_base_t {
	    DECLARE_CONCRETE_RTTLITE (LaunchAgent, gofer_base_t);

	//  Server
	public:
	    class Vca_API Server : public VRolePlayer {
		DECLARE_CONCRETE_RTTLITE (Server, VRolePlayer);

	    //  Construction
	    public:
		Server (
		    VString const &rLaunchCommand
		) : m_iLaunchCommand (rLaunchCommand), m_pIPassiveServer (this) {
		}

	    //  Destruction
	    private:
		~Server ();

	    //  Roles
	    public:
		using BaseClass::getRole;

	    //  IPassiveServer Role
	    private:
		VRole<ThisClass,IPassiveServer> m_pIPassiveServer;
	    public:
		void getRole (IPassiveServer::Reference &rpRole) {
		    m_pIPassiveServer.getRole (rpRole);
		}

	    //  IPassiveServer Callbacks
	    public:
		void RequestCallback (
		    IPassiveServer *pRole, IPassiveCall *pCall, VString const &rCallerAddress, uuid_t const &rCallID
		);

	    //  State
	    private:
		VString const m_iLaunchCommand;
	    };

	//  Construction
	public:
	    template <typename command_t> LaunchAgent (command_t iCommand) : m_iCommand (iCommand) {
	    }

	//  Destruction
	private:
	    ~LaunchAgent ();

	//  Callbacks
	protected:
	    void onNeed () OVERRIDE {
		m_iCommand.materializeFor (this);
		BaseClass::onNeed ();
	    }
	    void onData () OVERRIDE {
		IPassiveServer::Reference pIPassiveServer;
		(new Server (m_iCommand))->getRole (pIPassiveServer);
		setTo (pIPassiveServer);
	    }

	//  State
	private:
	    VInstanceOf_String m_iCommand;
	};
    }
}

#endif
