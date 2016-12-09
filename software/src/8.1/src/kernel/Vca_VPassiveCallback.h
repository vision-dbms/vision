#ifndef Vca_VPassiveCallback_Interface
#define Vca_VPassiveCallback_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGofer.h"

#include "Vca_IAckReceiver.h"
#include "Vca_IPassiveCall.h"
#include "Vca_IPassiveCallback.h"

#include "Vca_VInstanceOfInterface.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IPassiveClient.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VPassiveCallback : public VGofer {
	DECLARE_CONCRETE_RTTLITE (VPassiveCallback, VGofer);

    //  Construction
    public:
	template <class caller_t, class controller_t> VPassiveCallback (
	    VRolePlayer *pOffering, IPassiveCall *pCall, uuid_t const &rCallID, caller_t *pCaller, controller_t *pController
	) : m_pOffering (pOffering), m_pCall (pCall), m_iCallID (rCallID), m_pCaller (pCaller), m_pController (pController), m_pIPassiveCallback (this) {
	    retain (); {
		onNeed ();
	    } untain ();
	}
	template <class caller_t> VPassiveCallback (
	    VRolePlayer *pOffering, IPassiveCall *pCall, uuid_t const &rCallID, caller_t *pCaller
	) : m_pOffering (pOffering), m_pCall (pCall), m_iCallID (rCallID), m_pCaller (pCaller), m_pController (
	    static_cast<IAckReceiver*>(0)
	), m_pIPassiveCallback (this) {
	    retain (); {
		onNeed ();
	    } untain ();
	}

    //  Destruction
    private:
	~VPassiveCallback ();

    //  Roles
    private:
	using BaseClass::getRole;

    //  IPassiveCallback
    private:
	VRole<ThisClass,IPassiveCallback> m_pIPassiveCallback;
    public:
	void getRole (IPassiveCallback::Reference &rpRole) {
	    m_pIPassiveCallback.getRole (rpRole);
	}

    //  IPassiveCallback Methods
    public:
	void QueryService (IPassiveCallback *pRole, IObjectSink *pObjectSink, VTypeInfo *pObjectType);

    //  Triggers and Callbacks
    private:
	void onNeed ();
	void onData ();
	void onError (IError *pIError, VString const &rMessage);

    //  State
    private:
	VRolePlayer::Reference		const	m_pOffering;
	uuid_t				const	m_iCallID;
	IPassiveCall::Reference			m_pCall;
	VInstanceOfInterface<IPassiveClient>	m_pCaller;
	VInstanceOfInterface<IAckReceiver>	m_pController;
    };
}


#endif
