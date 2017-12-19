/*****  Vca Distribution Client Example  *****/

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

#include "Vca_VClientApplication.h"

#include "Vca_VLineGrabber.h"

#include "vca_samples_ipublication.h"
#include "vca_samples_isubscription.h"


/*********************************
 *********************************
 *****                       *****
 *****  VcaSamples::ThisApp  *****
 *****                       *****
 *********************************
 *********************************/

namespace VcaSamples {
    class ThisApp : public Vca::VClientApplication {
	DECLARE_CONCRETE_RTT (ThisApp, Vca::VClientApplication);

    //  Aliases
    public:
	typedef Vca::VLineGrabber_<ThisClass> InputSource;

	typedef IVReceiver<VString const&> IRecipient;
	typedef IVReceiver<ISubscription*> ISubscriber;

    //  Construction
    public:
	ThisApp (Context *pContext);

    //  Destruction
    private:
	~ThisApp ();

    //  Roles
    public:
	using BaseClass::getRole;

    //--->  IRecipient (Data Receiver) Role
    private: //  ... form for templated interfaces
	Vca::VRole<ThisClass,IRecipient > m_pIRecipient;
    public:
	void getRole (IRecipient::Reference &rpRole) {
	    m_pIRecipient.getRole (rpRole);
	}

    //--->  ISubscriber (Subscription Receiver) Role
    private: //  ... form for templated interfaces
	Vca::VRole<ThisClass,ISubscriber> m_pISubscriber;
    public:
	void getRole (ISubscriber::Reference &rpRole) {
	    m_pISubscriber.getRole (rpRole);
	}

    //  Role Callbacks
    //--->  IRecipient
    public:
	void OnData (IRecipient *pRole, VString const &rString);

    //--->  ISubscriber
    public:
	void OnData (ISubscriber *pRole, ISubscription *pSubscription);

    //--->  (Other : Vca::IClient)
    private:
	void OnError_(Vca::IError *pError, VString const &rMessage);

    //  Startup
    private:
	bool start_();

	void onPublication (IPublication *pPublication);
	void onStandardInput (Vca::VBSProducer *pBS);

    //  Input Processing
    public:
	bool onInputLine (char const *pLine, size_t sLine);
	void onInputDone ();

    //  State
    private:
	ISubscription::Reference m_pSubscription;
    };
}

template class Vca::VLineGrabber_<VcaSamples::ThisApp>;


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VcaSamples::ThisApp);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VcaSamples::ThisApp::ThisApp (Context *pContext) : BaseClass (pContext), m_pISubscriber (this), m_pIRecipient (this) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VcaSamples::ThisApp::~ThisApp () {
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void VcaSamples::ThisApp::OnData (IRecipient *pRole, VString const &rString) {
    printf ("+++ Got %s\n", rString.content ());
}

void VcaSamples::ThisApp::OnData (ISubscriber *pRole, ISubscription *pSubscription) {
    m_pSubscription.setTo (pSubscription);

    fprintf (stderr, "+++ Got Subscription: %p\n", pSubscription);
}

void VcaSamples::ThisApp::OnError_(Vca::IError *pError, VString const &rMessage) {
    printf ("+++ Got Error: %s\n", rMessage.content ());

    setExitStatus (ErrorExitValue);
    stop ();
}


/*********************
 *********************
 *****  Startup  *****
 *********************
 *********************/

bool VcaSamples::ThisApp::start_() {
    if (!BaseClass::start_())
	return false;

    if (getServerInterface (this, &ThisClass::onPublication))
	getStandardInput ();
    else {
	raiseApplicationException ("No Server To Contact");
	setExitStatus (ErrorExitValue);
    }

    return isStarting ();
}

void VcaSamples::ThisApp::onPublication (IPublication *pPublication) {
    if (pPublication) {
	ISubscriber::Reference pSubscriber;
	getRole (pSubscriber);

	IRecipient::Reference pRecipient;
	getRole (pRecipient);

	pPublication->Subscribe (pSubscriber, pRecipient, false);
    } else {
	fprintf (stderr, "+++ No Server At Address %s\n", arg0 ());
	setExitStatus (ErrorExitValue);
	stop ();
    }
}

void VcaSamples::ThisApp::onStandardInput (Vca::VBSProducer *pStdin) {
    InputSource::Reference pInputProvider (new InputSource (pStdin, this));
}


/******************************
 ******************************
 *****  InputProcessing  *****
 ******************************
 ******************************/

bool VcaSamples::ThisApp::onInputLine (char const *pLine, size_t sLine) {
    return true;
}

void VcaSamples::ThisApp::onInputDone () {
    stop ();
}


/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int argc, char *argv[]) {
    Vca::VOneAppMain_<VcaSamples::ThisApp> iMain (argc, argv);
    return iMain.processEvents ();
}
