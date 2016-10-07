/*****  Vca Distribution Server Example  *****/

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

#include "Vca_VServerApplication.h"

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
    class ThisApp : public Vca::VServerApplication {
	DECLARE_CONCRETE_RTT (ThisApp, Vca::VServerApplication);

    //  Aliases
    public:
	typedef Reference AppReference;
	typedef Vca::VLineGrabber_<ThisClass> InputSource;

    //  Subscription
    public:
	class Subscription : public Vca::VRolePlayer {
	    DECLARE_CONCRETE_RTT (Subscription, Vca::VRolePlayer);

	//  Aliases
	public:
	    typedef IVReceiver<VString const &> IRecipient;

	//  Construction
	public:
	    Subscription (ThisApp *pApp, ISubscriber *pSubscriber, IRecipient *pRecipient, bool bSuspended);

	//  Destruction
	private:
	    ~Subscription ();

	//  Roles
	public:
	    using BaseClass::getRole;

	//--->  ISubscription Role
	private:
	    Vca::VRole<ThisClass,ISubscription> m_pISubscription;
	public:
	    void getRole (ISubscription::Reference &rpRole) {
		m_pISubscription.getRole (rpRole);
	    }

	//  Role Callbacks
	//--->  ISubscription
	public:
	    void Suspend (ISubscription *pRole);
	    void Resume (ISubscription *pRole);
	    void Cancel (ISubscription *pRole);

	//  Publication
	public:
	    ThisClass *publish (VString const &rMessage);

	//  Maintenance
	private:
	    void unlink ();

	//  State
	private:
	    AppReference 		const m_pApp;
	    ISubscriber::Reference	const m_pSubscriber;
	    IRecipient::Reference	const m_pRecipient;
	    Pointer	m_pSuccessor;
	    Pointer	m_pPredecessor;
	    bool	m_bSuspended;
	    bool	m_bCanceled;
	};
	friend class Subscription;

    //  Construction
    public:
	ThisApp (Context *pContext);

    //  Destruction
    private:
	~ThisApp ();

    //  Roles
    public:
	using BaseClass::getRole;

    //--->  IPublication Role
    private:
	Vca::VRole<ThisClass,IPublication> m_pIPublication;
    public:
	void getRole (IPublication::Reference &rpRole) {
	    m_pIPublication.getRole (rpRole);
	}

    //  Role Callbacks
    //--->  IPublication
    public:
	void Subscribe (
	    IPublication *pRole, ISubscriber *pSubscriber, IRecipient *pRecipient, bool bSuspended
	);

    //  Startup
    private:
	void onStandardInput (Vca::VBSProducer *pStdin);
	bool start_();

    //  Input Processing
    public:
	bool onInputLine (char const *pLine, size_t sLine);
	void onInputDone ();

    //  State
    private:
	Subscription::Pointer m_pSubscriptions;
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

VcaSamples::ThisApp::ThisApp (Context *pContext) : BaseClass (pContext), m_pIPublication (this) {
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

void VcaSamples::ThisApp::Subscribe (
    IPublication *pRole, ISubscriber *pSubscriber, IRecipient *pRecipient, bool bSuspended
) {
    if (pSubscriber) {
	Subscription::IRecipient::Reference pMessageReceiver (
	    dynamic_cast<Subscription::IRecipient*>(pRecipient)
	);
	if (pMessageReceiver.isNil ())
	    pSubscriber->OnError (0, "Invalid or Missing Recipient");
	else {
	    Subscription::Reference pSubscription (
		new Subscription (this, pSubscriber, pMessageReceiver, bSuspended)
	    );
	}
    }
}


/*********************
 *********************
 *****  Startup  *****
 *********************
 *********************/

bool VcaSamples::ThisApp::start_() {
    if (!BaseClass::start_())
	return false;

    if (offerSelf ())
	getStandardInput ();
    else {
	fprintf (stderr, "Usage: No address to offer object.\n");
	setExitStatus (ErrorExitValue);
    }

    return isStarting ();
}

void VcaSamples::ThisApp::onStandardInput (Vca::VBSProducer *pStdin) {
    InputSource::Reference pInputProvider (new InputSource (pStdin, this));
}


/******************************
 ******************************
 *****  Input Processing  *****
 ******************************
 ******************************/

bool VcaSamples::ThisApp::onInputLine (char const *pLine, size_t sLine) {
    VString iText;
    iText.setTo (pLine, sLine);

    Subscription::Reference pSubscription (m_pSubscriptions);
    while (pSubscription) {
	pSubscription.setTo (pSubscription->publish (iText));
    }

    return true;
}

void VcaSamples::ThisApp::onInputDone () {
    stop ();
}


/***********************************************
 ***********************************************
 *****                                     *****
 *****  VcaSamples::ThisApp::Subscription  *****
 *****                                     *****
 ***********************************************
 ***********************************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (VcaSamples::ThisApp::Subscription);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VcaSamples::ThisApp::Subscription::Subscription (
    ThisApp *pApp, ISubscriber *pSubscriber, IRecipient *pRecipient, bool bSuspended
) : m_pApp (pApp)
  , m_pSubscriber (pSubscriber)
  , m_pRecipient (pRecipient)
  , m_pSuccessor (pApp->m_pSubscriptions)
  , m_bSuspended (bSuspended)
  , m_bCanceled (false)
  , m_pISubscription (this)
{
    pApp->m_pSubscriptions.setTo (this);

    retain (); {
	ISubscription::Reference pRole;
	getRole (pRole);
	pSubscriber->OnData (pRole);
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VcaSamples::ThisApp::Subscription::~Subscription () {
    unlink ();
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void VcaSamples::ThisApp::Subscription::Suspend (ISubscription *pRole) {
    if (!m_bCanceled)
	m_bSuspended = true;
}

void VcaSamples::ThisApp::Subscription::Resume (ISubscription *pRole) {
    if (!m_bCanceled)
	m_bSuspended = false;
}

void VcaSamples::ThisApp::Subscription::Cancel (ISubscription *pRole) {
    unlink ();
}

/*************************
 *************************
 *****  Publication  *****
 *************************
 *************************/

VcaSamples::ThisApp::Subscription *VcaSamples::ThisApp::Subscription::publish (VString const &rMessage) {
    if (m_pRecipient && !m_bSuspended)
	m_pRecipient->OnData (rMessage);
    return m_pSuccessor;
}

/*************************
 *************************
 *****  Maintenance  *****
 *************************
 *************************/

void VcaSamples::ThisApp::Subscription::unlink () {
    m_bCanceled = m_bSuspended = true;

    if (m_pSuccessor)
	m_pSuccessor->m_pPredecessor.setTo (m_pPredecessor);
    if (m_pPredecessor)
	m_pPredecessor->m_pSuccessor.setTo (m_pSuccessor);
    else
	m_pApp->m_pSubscriptions.setTo (m_pSuccessor);

    m_pPredecessor.clear ();
    m_pSuccessor.clear ();
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
