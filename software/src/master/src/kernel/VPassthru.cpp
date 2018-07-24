/*****  VPassthru Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VOneAppMain_.h"

#include "Vca_VBSConnector.h"
#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VClientApplication.h"
#include "Vca_VDeviceFactory.h"

#include "Vca_VPassiveAgent.h"
#include "Vca_IPassiveServer.h"

#include "VkStatus.h"


/***********************
 ***********************
 *****             *****
 *****  VPassthru  *****
 *****             *****
 ***********************
 ***********************/

class VPassthru : public Vca::VClientApplication {
    DECLARE_CONCRETE_RTTLITE (VPassthru, Vca::VClientApplication);

//  Aliases
public:
    typedef V::uuid_t		uuid_t;
    typedef V::VString		VString;

    typedef Vca::IError		IError;
    typedef Vca::IPassiveCall	IPassiveCall;
    typedef Vca::IPassiveServer	IPassiveServer;

    typedef Vca::VBSConnector	VBSConnector;
    typedef Vca::VBSConsumer	VBSConsumer;
    typedef Vca::VBSProducer	VBSProducer;

    typedef Vca::VDeviceFactory	VDeviceFactory;

    typedef Vca::VPassiveAgent	VPassiveAgent;

    typedef VPassiveAgent::CallbackRequest AgentCallbackRequest;

//  CallbackRequest
public:
    class CallbackRequest : public Activity, public AgentCallbackRequest {
	DECLARE_CONCRETE_RTTLITE (CallbackRequest, Activity);

    //  Aliases
    public:
	typedef Reference CallbackRequestReference;

    //  Call
    public:
	class Call : virtual public VRolePlayer {
	    DECLARE_CONCRETE_RTTLITE (Call, VRolePlayer);

	//  Construction
	public:
	    Call (CallbackRequest *pRequest, IPassiveServer *pCallbackServer);

	//  Destruction
	private:
	    ~Call ();

	//  IPassiveCall Role
	private:
	    Vca::VRole<ThisClass,IPassiveCall> m_pIPassiveCall;
	public:
	    void getRole (IPassiveCall::Reference &rpRole) {
		m_pIPassiveCall.getRole (rpRole);
	    }

	//  IPassiveCall Callbacks
	public:
	    void GetUUID (IPassiveCall *pRole, IVReceiver<uuid_t const&> *pReceiver);
	    virtual void OnError_(IError *pInterface, VString const &rMessage) OVERRIDE;

	//  State
	private:
	    CallbackRequestReference const m_pRequest;
	};
	friend class Call;

    //  Construction
    public:
	CallbackRequest (
	    VPassthru *pApplication, AgentCallbackRequest const &rCallbackData, IPassiveServer *pCallbackServer
	);

    //  Destruction
    private:
	~CallbackRequest ();

    //  Access
    protected:
	virtual void getDescription_(VString &rDescription) const OVERRIDE;

    //  Management
    protected:
	using BaseClass::onSuccess;
	using BaseClass::onFailure;
    private:
	void onSuccess (Call *pCall);
	void onFailure (Call *pCall, IError *pInterface, VString const &rMessage);
    };

//  Construction
public:
    VPassthru (Context *pContext);

//  Destruction
private:
    ~VPassthru ();

//  Callbacks
private:
    virtual bool start_() OVERRIDE;

private:
    void onCallbackError (VString const &rMessage);
    void onCallbackServer (IPassiveServer *pPassiveServer);
};


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VPassthru::VPassthru (Context *pContext) : BaseClass (pContext) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VPassthru::~VPassthru () {
}


/***********************
 ***********************
 *****  Operation  *****
 ***********************
 ***********************/

bool VPassthru::start_() {
    if (!BaseClass::start_())
	return false;

    bool const bForwardingACallback = commandStringValue ("callback");

    VString iDestination;
    if (bForwardingACallback && !getServerInterface (this, &ThisClass::onCallbackServer) || !getServerName (iDestination)) {
	displayUsagePatterns (
    	    "<destination-options> <trace-options>",
	    static_cast<char const*>(0)
	);
	displayOptions (
	    "  Destination Options:",
	    "     -server=<destination> | -serverFile=<destinationNameFile>",
	    static_cast<char const*>(0)
	);
	displayStandardOptions ();
	setExitStatus (ErrorExitValue);
    } else if (!bForwardingACallback) {
	VkStatus iStatus;
	VBSProducer::Reference pDstBSProducer, pStdBSProducer;
	VBSConsumer::Reference pDstBSConsumer, pStdBSConsumer;
	if (VDeviceFactory::Supply (iStatus, pDstBSProducer, pDstBSConsumer, iDestination, Vca::Default::TcpNoDelaySetting ())
	&&  VDeviceFactory::SupplyStdToHere (iStatus, pStdBSProducer)
	&&  VDeviceFactory::SupplyStdToPeer (iStatus, pStdBSConsumer)
	) {
	    incrementActivityCount ();
 	    VBSConnector::Reference pConnector;
 	    pConnector.setTo (new VBSConnector (pDstBSProducer, pStdBSConsumer));
 	    pConnector.setTo (new VBSConnector (pStdBSProducer, pDstBSConsumer));
	}
    }

    return isStarting ();
}

void VPassthru::onCallbackError (VString const &rMessage) {
    fprintf (stderr, ">>> OnCallback: %s\n", rMessage.content ());
    stop ();
}

void VPassthru::onCallbackServer (IPassiveServer *pCallbackServer) {
    VPassiveAgent::CallbackRequest iCallbackRequest (commandStringValue ("callback"));
    if (!pCallbackServer)
	onCallbackError ("Callback Not Supported");
    else if (iCallbackRequest.isntValid ())
	onCallbackError ("Callback Request Invalid");
    else {
	(new CallbackRequest (this, iCallbackRequest, pCallbackServer))->discard ();
    }
    decrementActivityCount (); // ... ends activity started by 'getServerInterface'
}


/****************************************
 ****************************************
 *****                              *****
 *****  VPassthru::CallbackRequest  *****
 *****                              *****
 ****************************************
 ****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VPassthru::CallbackRequest::CallbackRequest (
    VPassthru *pApplication, AgentCallbackRequest const &rCallbackData, IPassiveServer *pCallbackServer
) : BaseClass (pApplication), AgentCallbackRequest (rCallbackData) {
    retain (); {
	onStart ();
	(new Call (this, pCallbackServer))->discard ();
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VPassthru::CallbackRequest::~CallbackRequest () {
}


void VPassthru::CallbackRequest::getDescription_(VString &rDescription) const {
    rDescription << "vpassthru callback ";
    getCallIDAsString (rDescription);
    rDescription << "@" << callerAddress ();
}

/************************
 ************************
 *****  Management  *****
 ************************
 ************************/

void VPassthru::CallbackRequest::onSuccess (Call *pCall) {
    onSuccess ();
    onDone ();
}

void VPassthru::CallbackRequest::onFailure (Call *pCall, IError *pInterface, VString const &rMessage) {
    onFailure (pInterface, rMessage);
    onDone ();
}


/**********************************************
 **********************************************
 *****                                    *****
 *****  VPassthru::CallbackRequest::Call  *****
 *****                                    *****
 **********************************************
 **********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VPassthru::CallbackRequest::Call::Call (
    CallbackRequest *pRequest, IPassiveServer *pCallbackServer
) : m_pRequest (pRequest), m_pIPassiveCall (this) {
    retain (); {
	IPassiveCall::Reference pThisCall;
	getRole (pThisCall);
	pCallbackServer->RequestCallback (pThisCall, pRequest->callerAddress (), pRequest->callID ());
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VPassthru::CallbackRequest::Call::~Call () {
    m_pRequest->onSuccess (this);
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void VPassthru::CallbackRequest::Call::GetUUID (IPassiveCall *pRole, IVReceiver<uuid_t const&> *pReceiver) {
    if (pReceiver)
	pReceiver->OnData (m_pRequest->callID ());
}

void VPassthru::CallbackRequest::Call::OnError_(IError *pInterface, VString const &rMessage) {
    m_pRequest->onFailure (this, pInterface, rMessage);
}


/**************************
 **************************
 *****                *****
 *****  Main Program  *****
 *****                *****
 **************************
 **************************/

int main (int argc, char *argv[]) {
    Vca::VOneAppMain_<VPassthru> iMain (argc, argv);
    return iMain.processEvents ();
}
