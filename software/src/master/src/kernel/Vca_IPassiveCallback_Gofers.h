#ifndef Vca_IPassiveCallback_Gofers_Interface
#define Vca_IPassiveCallback_Gofers_Interface

#ifndef Vca_IPassiveCallback_Gofers
#define Vca_IPassiveCallback_Gofers extern
#define Vca_IPassiveCallback_Gofers_API
#else
#define Vca_IPassiveCallback_Gofers_API Vca_API
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInterface.h"

#include "Vca_IPassiveCallback.h"

#include "Vca_VInstanceOfInterface.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IPassiveConnector.h"
#include "Vca_IPassiveServer.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    DECLARE_GOFER_INTERFACE (Vca_IPassiveCallback_Gofers, Vca_IPassiveCallback_Gofers_API, IPassiveCallback)

    namespace IPassiveCallback_Gofer {
	typedef VGoferInterface<IPassiveCallback> gofer_base_t;

	class Vca_API Request : public gofer_base_t {
	    DECLARE_CONCRETE_RTTLITE (Request, gofer_base_t);

	//  Construction
	public:
	    template <class connector_t, class server_t, typename timeout_t> Request (
		connector_t pConnector, server_t pServer, timeout_t sTimeout
	    ) : m_pConnector (pConnector), m_pServer (pServer), m_sTimeout (sTimeout), m_pIPassiveCallbackReceiver (this) {
	    }

	//  Destruction
	private:
	    ~Request () {
	    }

	//  Roles
	private:
	    using BaseClass::getRole;

	//  IPassiveCallbackReceiver
	private:
	    VRole<ThisClass,IPassiveCallbackReceiver> m_pIPassiveCallbackReceiver;
	    void getRole (IPassiveCallbackReceiver::Reference &rpRole) {
		m_pIPassiveCallbackReceiver.getRole (rpRole);
	    }

	//  IPassiveCallbackReceiver Methods
	public:
	    void OnData (IPassiveCallbackReceiver *pRole, IPassiveCallback *pCallback) {
		BaseClass::setTo (pCallback);
	    }

	//  Callbacks
	private:
	    virtual void onNeed () OVERRIDE {
		m_pServer.materializeFor (this);
		m_pConnector.materializeFor (this);
		m_sTimeout.materializeFor (this);
		BaseClass::onNeed ();
	    }
	    virtual void onData() OVERRIDE {
		IPassiveCallbackReceiver::Reference pCallbackSink;
		getRole (pCallbackSink);
		m_pConnector->RequestCallback (pCallbackSink, m_pServer, m_sTimeout);
	    }

	//  State
	private:
	    VInstanceOfInterface<IPassiveConnector> m_pConnector;
	    VInstanceOfInterface<IPassiveServer>    m_pServer;
	    VInstanceOf<U64>			    m_sTimeout;
	};
    }
}

#endif
