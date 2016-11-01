/*****  Vca Tool Implementation  *****/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "vcatool.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VOneAppMain_.h"

#include "Vca_IPauseResume.h"
#include "Vca_IStop.h"
#include "Vca_IDie.h"

#include "Vca_VActivityMonitor.h"
#include "Vca_VActivitySink.h"

#include "Vca_VLineGrabber.h"

#include <iostream>

using namespace std;

namespace {
    using namespace Vca;
}


/********************
 ********************
 *****          *****
 *****  Gofers  *****
 *****          *****
 ********************
 ********************/

namespace Vca {
    namespace IRequest_Gofer {
	typedef VGoferInterface<IRequest> gofer_base_t;

	typedef IVReceiver<IRequest*> IRequestSink;

    /*****************************************************************/
	template <class Source_T, class Sink_T> class Subscription : public gofer_base_t {
	//  Aliases
	public:
	    typedef void (Source_T::*member_t)(IRequestSink*,Sink_T*,VMessageManager*);
	    typedef Subscription<Source_T,Sink_T> this_t;
	    DECLARE_CONCRETE_RTTLITE (this_t, gofer_base_t);

	//  Construction
	public:
	    template <class source_t, class sink_t> Subscription (
		source_t *pSource, sink_t *pSink, member_t pMember
	    ) : m_pSource (pSource), m_pSink (pSink), m_pMember (pMember), m_pIRequestSink (this) {
	    }

	//  Destruction
	private:
	    ~Subscription () {
	    }

	//  Roles
	private:
	    VRole<ThisClass,IRequestSink> m_pIRequestSink;

	//  Callbacks
	public:
	    void OnData (IRequestSink *pRole, IRequest *pRequest) {
		BaseClass::setTo (pRequest);
	    }
	private:
	    void onNeed () {
		m_pSource.materializeFor (this);
		m_pSink.materializeFor (this);
		BaseClass::onNeed ();
	    }
	    void onData () {
		IRequestSink::Reference pRequestSink;
		m_pIRequestSink.getRole (pRequestSink);
		submit (pRequestSink, m_pSource, m_pSink);
	    }

	//  Implementation
	private:
	    void submit (IRequestSink *pRequestSink, Source_T *pSource, Sink_T *pSink) {
		if (pSource)
		    (pSource->*m_pMember)(pRequestSink, pSink, 0);
		else {
		    BaseClass::onError (0, "Activity Data Not Available");
		}
	    }

	//  State
	private:
	    VInstanceOfInterface<Source_T> m_pSource;
	    VInstanceOfInterface<Sink_T> m_pSink;
	    member_t const m_pMember;
	};

    /*****************************************************************/
	template <
	    typename source_t, typename sink_t, class Source_T, class Sink_T
	> inline Subscription<Source_T,Sink_T> *NewSubscription (
	    source_t *pSource, sink_t *pSink, void (Source_T::*pMember)(IRequestSink*,Sink_T*,VMessageManager*)
	) {
	    typedef void (Source_T::*member_t)(IRequestSink*,Sink_T*,VMessageManager*);
	//  The seemingly pointless 'static_cast<T*>(pT)' in the following expression actuals
	//  serves to suppress the '&' that C++ would otherwise include (eg, T*&) in the type
	//  used to instantiate Subscription2's templated constructor...
	    return new Subscription<Source_T,Sink_T>(
		static_cast<source_t*>(pSource), static_cast<sink_t*>(pSink), static_cast<member_t>(pMember)
	    );
	}
    }
}


/************************
 ************************
 *****              *****
 *****  ... Action  *****
 *****              *****
 ************************
 ************************/

namespace {
    template <typename Interface_T> class Action : public Tool::Activity {
	DECLARE_CONCRETE_RTTLITE (Action<Interface_T>, Tool::Activity);

    //  Aliases
    public:
	typedef VGoferInterface<Interface_T> gofer_t;
	typedef void (Tool::*member_t)(Interface_T*);

    //  Construction
    public:
	Action (
	    Tool *pApplication, member_t pMember, gofer_t *pGofer = 0
	) : BaseClass (pApplication), m_pMember (pMember) {
	    retain (); {
		if (pGofer) {
		    pGofer->supplyMembers (this, &ThisClass::onValue, &ThisClass::onError);
		} else {
		    BaseClass::getServerInterface (this, &ThisClass::onValue, &ThisClass::onError);
		}
	    } untain ();
	}

    //  Destruction
    private:
	~Action () {
	}

    //  Access
    public:
	Tool *tool () const {
	    return static_cast<Tool*>(application ());
	}

    //  Callbacks
    private:
	void onValue (Interface_T *pInterface) {
	    (tool()->*m_pMember)(pInterface);
	    onDone ();
	}
	void onError (IError *pInterface, VString const &rMessage) {
	    onDone ();
	}

    //  State
    private:
	member_t const m_pMember;
    };
}


/*****************************
 *****************************
 *****                   *****
 *****  ... DataRequest  *****
 *****                   *****
 *****************************
 *****************************/

namespace {
    template <typename Val_T> class DataRequest : public Tool::Activity {
	DECLARE_CONCRETE_RTTLITE (DataRequest<Val_T>, Tool::Activity);

    //  Aliases
    public:
	typedef IVReceiver<Val_T> IDataSink;
	typedef IVReceiver<IRequest*> ITicketSink;

    //  Construction
    public:
	template <typename gofer_t> DataRequest (
	    Tool *pApplication, VString const &rGetterKey, gofer_t *pGetterGofer, ssid_t xObject = 0
	) : BaseClass (pApplication), m_iGetterKey (rGetterKey), m_xObject (xObject), m_pIDataSink (this), m_pITicketSink (this) {
	    retain (); {
		if (pGetterGofer)
		    pGetterGofer->supplyMembers (this, &ThisClass::onGetter, &ThisClass::onError);
		else {
		    typedef void (ThisClass::*member_t)(typename gofer_t::interface_t*);
		    member_t const pMember = &ThisClass::onGetter;
		    getServerInterface (this, pMember, &ThisClass::onError);
		}
	    } untain ();
	}

    //  Destruction
    private:
	~DataRequest () {
	}

    //  Access
    public:
	VString description () const {
	    if (!m_xObject)
		return m_iGetterKey;

	    return VString () << m_xObject << "[" << m_iGetterKey << "]";
	}

    //  IDataSink Role
    private:
	VRole<ThisClass, IDataSink> m_pIDataSink;
    public:
	void getRole (typename IDataSink::Reference &rpRole) {
	    m_pIDataSink.getRole (rpRole);
	}

    //  IDataSink Callbacks
    public:
	void OnData (IDataSink *pRole, Val_T data) {
            cout << description () << ": " << data << endl ;
            onDone ();
	}

    //  ITicketSink Role
    private:
	VRole<ThisClass, ITicketSink> m_pITicketSink;
    public:
	void getRole (typename ITicketSink::Reference &rpRole) {
	    m_pITicketSink.getRole (rpRole);
	}

    //  ITicketSink Callbacks
    public:
	void OnData (ITicketSink *pRole, IRequest *pTicket) {
	    static_cast<Tool*>(application ())->createTracker (pTicket, description ());
            onDone ();
	}

    // Other Callbacks
    public:
	void OnError_(IError *pInterface, VString const &rMessage) {
	    cerr << "Error: " << rMessage << endl;
            onDone ();
	}

    private:
	void get (IGetter_Ex2 *s, IVReceiver<VString const &> *r, VString const &k) {
	    s->GetStringValue( r, k ) ;
	}

	void get (IGetter_Ex2 *s, IVReceiver<U32> *r, VString const &k) {
	    s->GetU32Value( r, k ) ;
	}

	void get (IGetter_Ex2 *s, IVReceiver<U64> *r, VString const &k) {
	    s->GetU64Value( r, k ) ;
	}

	void get (IGetter_Ex2 *s, IVReceiver<F32> *r, VString const &k) {
	    s->GetF32Value( r, k ) ;
	}

	void get (IGetter_Ex2 *s, IVReceiver<F64> *r, VString const &k) {
	    s->GetF64Value( r, k ) ;
	}

	void onGetter (IGetter_Ex2 *pSupplier) {
	    if (!pSupplier) {
		cerr << "Extended query interface not supported" << endl;
		onDone () ;
	    } else {
		typename IDataSink::Reference pDataReceiverRole;
		getRole (pDataReceiverRole);
            // Make Request Here...
                get (pSupplier, pDataReceiverRole, m_iGetterKey);
	    }
	}
	void onGetter (IPeek *pSupplier) {
	    if (!pSupplier) {
		cerr << "Extended query interface not supported" << endl;
		onDone () ;
	    } else {
		typename IDataSink::Reference pDataSink;
		getRole (pDataSink);

		ITicketSink::Reference pTicketSink;
		getRole (pTicketSink);

            // Make Request Here...
		pSupplier->GetValue (m_iGetterKey, pDataSink, pTicketSink);
	    }
	}
	void onError (IError* pInterface, VString const& rMessage) {
	    cerr << "Extended query interface error: " << rMessage << endl;
            onDone ();
	}

    private:
        VString const m_iGetterKey ;
	ssid_t  const m_xObject;
	VString m_iDescription;
    };
}


/*************************************
 *************************************
 *****                           *****
 *****  Vca::Tool::ActivitySink  *****
 *****                           *****
 *************************************
 *************************************/

namespace Vca {
    class Tool::ActivitySink : public VActivitySink {
	DECLARE_CONCRETE_RTTLITE (ActivitySink, VActivitySink);

    //  Aliases
    public:
	typedef V::VTime VTime;

    //********************************************************************************************/
    //  ActivityMonitor
    public:
	class ActivityMonitor : public VActivityMonitor {
	    DECLARE_CONCRETE_RTTLITE (ActivityMonitor, VActivityMonitor);

	//  Globals
	public:
	    static bool ShowingEvents ();

	//  Construction
	public:
	    ActivityMonitor (VActivity *pActivity);

	//  Destruction
	private:
	    ~ActivityMonitor ();

	//  Event Callbacks
	private:
	    void OnEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, VString const& rDescription);
	    void OnEndEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp);
	    void OnErrorEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, IError* pInterface, VString const& rMessage);
	    void OnFailureEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, IError* pInterface, VString const& rMessage);

	    void onCompletion_();

	//  State
	private:
	    bool m_bShowingEvents;
	};

    //********************************************************************************************/
    //  Construction
    public:
	ActivitySink (ActivityModel *pModel);

    //  Destruction
    private:
	~ActivitySink ();

    //  Activity Sink Callbacks
    private:
	void createMonitorFor (VActivity *pActivity) const;
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Tool::ActivitySink::ActivitySink (ActivityModel *pModel) : BaseClass (pModel) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Tool::ActivitySink::~ActivitySink () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::Tool::ActivitySink::createMonitorFor (VActivity *pActivity) const {
//  Create an ActivityMonitor object to record and track the activity...
    ActivityMonitor::Reference const pActivityMonitor (new ActivityMonitor (pActivity));
}


/******************************************************
 ******************************************************
 *****                                            *****
 *****  Vca::Tool::ActivitySink::ActivityMonitor  *****
 *****                                            *****
 ******************************************************
 ******************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

bool Vca::Tool::ActivitySink::ActivityMonitor::ShowingEvents () {
    return V::GetEnvironmentBoolean ("vcatool_showing_events");
}

Vca::Tool::ActivitySink::ActivityMonitor::ActivityMonitor (VActivity* pActivity) : BaseClass (pActivity), m_bShowingEvents (ShowingEvents ()) {
    retain (); {
	requestEvents ();
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Tool::ActivitySink::ActivityMonitor::~ActivityMonitor () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::Tool::ActivitySink::ActivityMonitor::OnEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, VString const& rDescription) {
    if (m_bShowingEvents)
	cout << activitySSID () << "[" << referenceCount () << "]: OnEvent: " << rDescription << endl;
}

void Vca::Tool::ActivitySink::ActivityMonitor::OnEndEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp) {
    if (m_bShowingEvents)
	cout << activitySSID () << "[" << referenceCount () << "]: OnEndEvent." << endl;
}

void Vca::Tool::ActivitySink::ActivityMonitor::OnErrorEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, IError* pInterface, VString const& rMessage) {
    if (m_bShowingEvents)
	cout << activitySSID () << "[" << referenceCount () << "]: OnErrorEvent: " << rMessage << endl;
}

void Vca::Tool::ActivitySink::ActivityMonitor::OnFailureEvent_(IEvent* pEvent, IEvent* pProxy, VTime const& rTimestamp, IError* pInterface, VString const& rMessage) {
    if (m_bShowingEvents)
	cout << activitySSID () << "[" << referenceCount () << "]: OnFailureEvent: " << rMessage << endl;
}

void Vca::Tool::ActivitySink::ActivityMonitor::onCompletion_() {
    if (m_bShowingEvents)
	cout << activitySSID () << "[" << referenceCount () << "]: OnCompletion." << endl;
}


/***************************
 ***************************
 *****                 *****
 *****  ... Tokenizer  *****
 *****                 *****
 ***************************
 ***************************/

namespace {
    char const g_iWhitespace[] = " \t\r\n";
    char const g_iRestOfLine[] = "\r\n";

    class Tokenizer {
	DECLARE_NUCLEAR_FAMILY (Tokenizer);

    //  Construction
    public:
	Tokenizer (VString const &rInput);

    //  Destruction
    public:
	~Tokenizer ();

    //  Use
    public:
	VString const &currentToken () const {
	    return m_iInput;
	}

	bool getToken (VString &rToken);
	bool getToken (unsigned int &rToken);
	bool getToken (unsigned __int64 &rToken);
	void showError (VString const &rWhat) const;
    private:
	bool skipToken ();

    //  State
    private:
	VString m_iInput;
    };

    Tokenizer::Tokenizer (VString const &rInput) : m_iInput (rInput) {
    }
    Tokenizer::~Tokenizer () {
    }
    bool Tokenizer::getToken (VString &rToken) {
	char   const*pToken = m_iInput + strspn (m_iInput, g_iWhitespace);
	size_t const sToken = strcspn (pToken, g_iWhitespace);
	rToken.setTo (pToken, sToken);

	m_iInput.setTo (pToken + sToken);

	return sToken > 0;
    }
    bool Tokenizer::getToken (unsigned int &rToken) {
	return 1 == sscanf (m_iInput, " %u ", &rToken) && skipToken ();
    }
    bool Tokenizer::getToken (unsigned __int64 &rToken) {
	return 1 == sscanf (m_iInput, " %llu ", &rToken) && skipToken ();
    }
    bool Tokenizer::skipToken () {
	char   const*pToken = m_iInput + strspn (m_iInput, g_iWhitespace);
	size_t const sToken = strcspn (pToken, g_iWhitespace);
	m_iInput.setTo (pToken + sToken);
	return true;
    }
    void Tokenizer::showError (VString const &rWhat) const {
	cerr << rWhat << endl;
    }
}


/************************************
 ************************************
 *****                          *****
 *****  Vca::Tool::Interpreter  *****
 *****                          *****
 ************************************
 ************************************/

namespace Vca {
    class Tool::Interpreter : public Activity {
	DECLARE_CONCRETE_RTTLITE (Interpreter, Activity);

	friend class VLineGrabber_<ThisClass>;

    //  Construction
    public:
	Interpreter (Tool *pTool, VBSProducer *pStdin);

    //  Destruction
    private:
	~Interpreter ();

    //  Access
    public:
	Tool *tool () const {
	    return static_cast<Tool*>(application());
	}
	ActivityModel *model () const {
	    return tool()->activityModel ();
	}

	template <class Interface_T> VGoferInterface<Interface_T> *getGoferFor (
	    VReference<VGoferInterface<Interface_T> > &rpGofer, ssid_t xObject, bool bAuthoritative
	) const {
	    return model ()->getGoferFor (rpGofer, xObject, bAuthoritative);
	}

    //  Callbacks
    private:
	bool onInputLine (char const *pLine, size_t sLine);
	void onInputDone ();

    //  Implementation Helpers
    private:
	template <class Interface_T> void invoke (
	    void (ThisClass::*pMember) (Tokenizer&,VGoferInterface<Interface_T>*,ssid_t),
	    Tokenizer &rTokenizer,
	    VReference<VGoferInterface<Interface_T> > &rpGofer,
	    bool bAuthoritative = true
	) {
	    rpGofer.clear ();

	    ssid_t xObject = 0;
	/*****************************************************************
	 *  In the following test, when:
	 *
	 *    !rTokenizer.getToken(xObject) ... apply the operation to the object specified
	 *                                      on 'vcatool's command line.
	 *    getGoferFor(...)              ... apply the operation to the object specified
	 *                                      by the value of 'xObject' returned (by name)
	 *                                      from 'rTokenizer.getToken (xObject)'.
	 *
	 *****************************************************************/
	    if (!rTokenizer.getToken (xObject) || getGoferFor (rpGofer, xObject, bAuthoritative))
		(this->*pMember) (rTokenizer,rpGofer,xObject);
	    else {
		showError (rTokenizer, xObject);
	    }
	}

	template <class Interface_T> void invoke (
	    void (Tool::*pMember) (Interface_T*), Tokenizer &rTokenizer, bool bAuthoritative = true
	) {
	    ssid_t xObject = 0; typename VGoferInterface<Interface_T>::Reference pGofer;
	/*****************************************************************
	 *  In the following test, when:
	 *
	 *    !rTokenizer.getToken(xObject) ... apply the operation to the object specified
	 *                                      on 'vcatool's command line.
	 *    getGoferFor(...)              ... apply the operation to the object specified
	 *                                      by the value of 'xObject' returned (by name)
	 *                                      from 'rTokenizer.getToken (xObject)'.
	 *
	 *****************************************************************/
	    if (!rTokenizer.getToken (xObject) || getGoferFor (pGofer, xObject, bAuthoritative))
		(new Action<Interface_T> (tool(), pMember, pGofer.referent ()))->discard ();
	    else {
		showError (rTokenizer, xObject);
	    }
	}

	void showError (Tokenizer const &rTokenizer, ssid_t xObject) const;

    //  Implementation
    private:
	void emitPrompt () const;

	template <typename gofer_t> void getImpl (Tokenizer &rTokenizer, gofer_t *pGofer, ssid_t xObject);
	void get      (Tokenizer &rTokenizer, VGoferInterface<IGetter_Ex2> *pGofer, ssid_t xObject);
	void get      (Tokenizer &rTokenizer, VGoferInterface<IPeek> *pGofer, ssid_t xObject);

	void show     (Tokenizer &rTokenizer);
	void showHelp (Tokenizer &rTokenizer);

	void deleat   (Tokenizer &rTokenizer);

	void showActivities ();
	void showTrackers ();
    };
}


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Tool::Interpreter::Interpreter (Tool *pTool, VBSProducer *pStdin) : BaseClass (pTool) {
    retain (); {
	(new VLineGrabber_<ThisClass> (pStdin, this))->discard ();
	emitPrompt ();
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Tool::Interpreter::~Interpreter () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

bool Vca::Tool::Interpreter::onInputLine (char const *pLine, size_t sLine) {
    Tokenizer iTokenizer (VString (pLine, sLine));
    
    VString iToken;
    while (iTokenizer.getToken (iToken)) {
	if (iToken.equalsIgnoreCase ("exit") || iToken.equalsIgnoreCase ("quit"))
	    return false;

	if (iToken.equalsIgnoreCase ("help")) {
	    showHelp (iTokenizer);
	} else if (iToken.equalsIgnoreCase ("show")) {
	    show (iTokenizer);
	} else if (iToken.equalsIgnoreCase ("delete")) {
	    deleat (iTokenizer);
	} else if (iToken.equalsIgnoreCase ("pause")) {
	    invoke (&Tool::sendPause, iTokenizer);
	} else if (iToken.equalsIgnoreCase ("resume")) {
	    invoke (&Tool::sendResume, iTokenizer);
	} else if (iToken.equalsIgnoreCase ("stop")) {
	    invoke (&Tool::sendStop, iTokenizer);
	} else if (iToken.equalsIgnoreCase ("hardstop")) {
	    invoke (&Tool::sendHardstop, iTokenizer);
	} else if (iToken.equalsIgnoreCase ("kill")) {
	    invoke (&Tool::sendDie, iTokenizer);
	} else if (iToken.equalsIgnoreCase ("get")) {
	    typedef void (ThisClass::*member_t) (Tokenizer&,VGoferInterface<IGetter_Ex2>*,ssid_t);
	    member_t const pMember = &ThisClass::get;
	    VGoferInterface<IGetter_Ex2>::Reference rpGofer;
	    invoke (pMember, iTokenizer, rpGofer);
	} else if (iToken.equalsIgnoreCase ("track")) {
	    typedef void (ThisClass::*member_t) (Tokenizer&,VGoferInterface<IPeek>*,ssid_t);
	    member_t const pMember = &ThisClass::get;
	    VGoferInterface<IPeek>::Reference rpGofer;
	    invoke (pMember, iTokenizer, rpGofer);
	} else {
	    cerr << iToken << "?" << endl;
	}
    }
    emitPrompt ();
    return true;
}

void Vca::Tool::Interpreter::onInputDone () {
    tool ()->deleteTrackers ();
    onDone ();
}

/****************************
 ****************************
 *****  Implementation  *****
 ****************************
 ****************************/

void Vca::Tool::Interpreter::showError (Tokenizer const &rTokenizer, ssid_t xObject) const {
    VString iMessage;
    iMessage.printf ("Object %u Not Found", xObject);
    rTokenizer.showError (iMessage);
}

void Vca::Tool::Interpreter::emitPrompt () const {
    cout << model ()->activityCount () << "> " << flush;
}

template <typename gofer_t> void Vca::Tool::Interpreter::getImpl (Tokenizer &rTokenizer, gofer_t *pGofer, ssid_t xObject) {
    VString iType; VString iKey;
    if (!rTokenizer.getToken (iType) || !rTokenizer.getToken (iKey)) {
    } else if (strcasecmp (iType, "u32") == 0) {
	(new DataRequest<U32> (tool (), iKey, pGofer, xObject))->discard ();
    } else if (strcasecmp (iType, "u64") == 0) {
	(new DataRequest<U64> (tool (), iKey, pGofer, xObject))->discard ();
    } else if (strcasecmp (iType, "f32") == 0) {
	(new DataRequest<F32> (tool (), iKey, pGofer, xObject))->discard ();
    } else if (strcasecmp (iType, "f64") == 0) {
	(new DataRequest<F64> (tool (), iKey, pGofer, xObject))->discard ();
    } else if (strcasecmp (iType, "string") == 0) {
	(new DataRequest<VString const&> (tool (), iKey, pGofer, xObject))->discard ();
    } else {
	cerr << "get " << iType << "?" << endl;
    }
}
template void Vca::Tool::Interpreter::getImpl (Tokenizer&,VGoferInterface<IGetter_Ex2>*,ssid_t);
template void Vca::Tool::Interpreter::getImpl (Tokenizer&,VGoferInterface<IPeek>*,ssid_t);

void Vca::Tool::Interpreter::get (Tokenizer &rTokenizer, VGoferInterface<IGetter_Ex2> *pGofer, ssid_t xObject) {
    return getImpl (rTokenizer, pGofer, xObject);
}

void Vca::Tool::Interpreter::get (Tokenizer &rTokenizer, VGoferInterface<IPeek> *pGofer, ssid_t xObject) {
    return getImpl (rTokenizer, pGofer, xObject);
}

void Vca::Tool::Interpreter::show (Tokenizer &rTokenizer) {
    VString iWhat;
    if (!rTokenizer.getToken (iWhat)) {
    } else if (iWhat.equalsIgnoreCase ("a") || iWhat.equalsIgnoreCase ("activities")) {
	showActivities ();
    } else if (iWhat.equalsIgnoreCase ("t") || iWhat.equalsIgnoreCase ("trackers")) {
	showTrackers ();
    } else {
	cerr << "show " << iWhat << "?" << endl;
    }
}

void Vca::Tool::Interpreter::deleat (Tokenizer &rTokenizer) {
    VString iWhat;
    if (!rTokenizer.getToken (iWhat)) {
    } else if (iWhat.equalsIgnoreCase ("t") || iWhat.equalsIgnoreCase ("tracker")) {
	Key::value_t xObject;
	if (rTokenizer.getToken (xObject)) {
	    Key iKey (xObject);
	    tool ()->deleteTracker (iKey);
	} else {
	    VString iMessage;
	    iMessage << "delete tracker '" << rTokenizer.currentToken () << "'?";
	    rTokenizer.showError (iMessage);
	}
    } else {
	cerr << "delete " << iWhat << "?" << endl;
    }
}

void Vca::Tool::Interpreter::showHelp (Tokenizer &rTokenizer) {
    printf ("Commands:\n");
    printf ("  help\n");
    printf ("\n");
    printf ("  pause [ssid#]\n");
    printf ("  resume [ssid#]\n");
    printf ("  stop [ssid#]\n");
    printf ("  hardstop [ssid#]\n");
    printf ("  kill [ssid#]\n");
    printf ("\n");
    printf ("  get [ssid#] {u32 | u64 | f32 | f64 | string} optionName\n");
    printf ("  track [ssid#] {u32 | u64 | f32 | f64 | string} optionName\n");
    printf ("\n");
    printf ("  show activities\n");
    printf ("  show trackers\n");
    printf ("\n");
    printf ("  delete tracker #\n");
}

void Vca::Tool::Interpreter::showActivities () {
    ActivityModel::Reference const pModel (model ());
    for (unsigned int xActivity = 0; xActivity < pModel->activityCount (); xActivity++) {
	ActivityModel::ActivityRecord::Reference pActivity (pModel->activityAtPosition (xActivity));
	if (pActivity) {
	    VString iStartTime;
	    pActivity->getRemoteStartTime (iStartTime);

	    VString iEndTime;
	    pActivity->getRemoteEndTime (iEndTime);

	    VString iDescription;
	    pActivity->getFormattedDescription (iDescription);

	    cout
		<< pActivity->ssid ()
		<< ": "
		<< iStartTime
		<< " "
		<< iEndTime
		<< endl
		<< "    "
		<< iDescription
		<< endl;
	}
    }
}

void Vca::Tool::Interpreter::showTrackers () {
    tool ()->showTrackers ();
}


/********************************
 ********************************
 *****                      *****
 *****  Vca::Tool::Tracker  *****
 *****                      *****
 ********************************
 ********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

static counter_t s_xNextTracker;

Vca::Tool::Tracker::Tracker (
    Tool *pTool, IRequest *pTicket, VString const &rDescription
) : BaseClass (pTool), m_iKey (s_xNextTracker++), m_pTicket (pTicket), m_iDescription (rDescription) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Tool::Tracker::~Tracker () {
    if (m_pTicket)
	m_pTicket->Cancel ();
    
}

/*********************
 *********************
 *****  Display  *****
 *********************
 *********************/

void Vca::Tool::Tracker::showSelf () const {
    cout << " " << keyValue () << ": " << m_iDescription;
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::Tool::Tracker::onMonitor () {
    tool ()->deleteTracker (m_iKey);
}


/***********************
 ***********************
 *****             *****
 *****  Vca::Tool  *****
 *****             *****
 ***********************
 ***********************/

/**********************************
 **********************************
 *****  Construction Helpers  *****
 **********************************
 **********************************/

Vca::Tool::IRequestGofer *Vca::Tool::ActivityGofer () {
    IRequestGofer *pResult = 0; {
	VGoferInterface<IActivitySource>::Reference pActivitySource;
	if (getInterfaceGofer (pActivitySource)) {
	    IActivitySink::Reference pActivitySink;
	    (new ActivitySink (m_pActivityModel))->getRole (pActivitySink);
	    pResult = IRequest_Gofer::NewSubscription (
		pActivitySource.referent (), pActivitySink.referent (), &IActivitySource::SupplyActivities
	    );
	}
    }
    return pResult;
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::Tool::Tool (Context *pContext)
    : BaseClass (pContext)
    , m_pActivityModel (new ActivityModel ())
    , m_pActivityGofer (ActivityGofer ())
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::Tool::~Tool () {
}


/*************************
 *************************
 *****  Transitions  *****
 *************************
 *************************/

namespace Vca {
    template< typename Val_T >
    bool Tool::launchActions (char const * key) {
        bool foundKey = false ;

        CommandCursor cursor (this) ;

        for (char const * keyValue = cursor.firstString (key)
                 ; keyValue
                 ; keyValue = cursor.nextString (key) ) {
        // No delete.  A reference to 'this' will be handed off to the server.
        // After the server calls back, it will drop its reference and the
        // request object will get cleaned up.
            (new DataRequest< Val_T > (this, keyValue, static_cast<VGoferInterface<IGetter_Ex2>*>(0)))->discard ();
        
            foundKey = true ;     
        }

        return foundKey ;
    }

    template bool Tool::launchActions< U32 > (char const *) ;
    template bool Tool::launchActions< U64 > (char const *) ;
    template bool Tool::launchActions< F32 > (char const *) ;
    template bool Tool::launchActions< F64 > (char const *) ;
    template bool Tool::launchActions< VString const & > (char const *) ;
}

bool Vca::Tool::start_() {
    if (!BaseClass::start_())
	return false;

    bool bUsedCorrectly = false;

    if (commandSwitchValue ("pause")) {
	(new Action<IPauseResume> (this, &ThisClass::sendPause))->discard ();
	bUsedCorrectly = true;
    }

    else if (commandSwitchValue ("resume")) {
	(new Action<IPauseResume> (this, &ThisClass::sendResume))->discard ();
	bUsedCorrectly = true;
    }

    else if (commandSwitchValue ("stop")) {
	(new Action<IStop> (this, &ThisClass::sendStop))->discard ();
	bUsedCorrectly = true;
    }

    else if (commandSwitchValue ("hardstop")) {
	(new Action<IStop> (this, &ThisClass::sendHardstop))->discard ();
	bUsedCorrectly = true;
    }

    else if (commandSwitchValue ("die")) {
	(new Action<IDie> (this, &ThisClass::sendDie))->discard ();
	bUsedCorrectly = true;
    }

    else if (commandSwitchValue ("interactive")) {
	incrementActivityCount ();
	getStandardInput ();
	bUsedCorrectly = true;
    }

    else {
    // Don't make this short circuit.  I want to run them all.
        bUsedCorrectly = ( launchActions< U32 > ("getU32") 
                           || bUsedCorrectly );
        bUsedCorrectly = ( launchActions< U64 > ("getU64") 
                           || bUsedCorrectly );
        bUsedCorrectly = ( launchActions< F32 > ("getF32") 
                           || bUsedCorrectly );
        bUsedCorrectly = ( launchActions< F64 > ("getF64") 
                           || bUsedCorrectly );
        bUsedCorrectly = ( launchActions< VString const & > ("getString") 
                           || bUsedCorrectly );
    }
    
    if (!bUsedCorrectly) {
	displayUsage ();
	setExitStatus (ErrorExitValue);
    }

    return isStarting ();
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::Tool::onStandardInput (VBSProducer *pStdin) {
    (new Interpreter (this, pStdin))->discard ();
    startActivityMonitor ();
    decrementActivityCount ();
}

void Vca::Tool::onStandardChannelAccessError (IError *pInterface, VString const &rMessage) {
    cerr << "No Standard Input: " << rMessage << endl;
    decrementActivityCount ();
}

void Vca::Tool::sendPause (IPauseResume *pInterface) {
    if (pInterface)
	pInterface->Pause ();
}

void Vca::Tool::sendResume (IPauseResume *pInterface) {
    if (pInterface)
	pInterface->Resume ();
}

void Vca::Tool::sendStop (IStop *pInterface) {
    if (pInterface)
	pInterface->Stop (false);
}

void Vca::Tool::sendHardstop (IStop *pInterface) {
    if (pInterface)
	pInterface->Stop (true);
}

void Vca::Tool::sendDie (IDie *pInterface) {
    if (pInterface)
	pInterface->Die ();
}


/************************
 ************************
 *****  Monitoring  *****
 ************************
 ************************/

void Vca::Tool::startActivityMonitor () {
    if (m_pActivityGofer)
	m_pActivityGofer->supplyMembers (
	    this, &ThisClass::onActivityRequestValue, &ThisClass::onActivityRequestError
	);
}

void Vca::Tool::onActivityRequestValue (IRequest *pRequest) {
    m_pActivityRequest.setTo (pRequest);
}

void Vca::Tool::onActivityRequestError (IError *pInterface, VString const &rMessage) {
    cerr << "+++ Activity Request Error: " << rMessage << endl;
}

void Vca::Tool::createTracker (IRequest *pTicket, VString const &rDescription) {
    Tracker::Reference const pNewTracker (new Tracker (this, pTicket, rDescription));
    unsigned int xNewTracker;
    m_sTrackers.Insert (pNewTracker->key (), xNewTracker);
    m_sTrackers[xNewTracker] = pNewTracker;
}

bool Vca::Tool::deleteTracker (Key const &rTrackerKey) {
    return m_sTrackers.Delete (rTrackerKey);
}

void Vca::Tool::deleteTrackers () {
    m_sTrackers.DeleteAll ();
}

void Vca::Tool::showTrackers () const {
    for (unsigned int xTracker = 0; xTracker < m_sTrackers.cardinality (); xTracker++) {
	m_sTrackers[xTracker]->showSelf ();
	cout << endl;
    }
}
/***********************
 ***********************
 *****  Reporting  *****
 ***********************
 ***********************/

void Vca::Tool::displayUsage () const {
    displayUsagePatterns (
	"-pause",
	"-resume",
	"-stop",
	"-hardstop",
        "-getString=key",
        "-getU32=key",
        "-getU64=key",
        "-getF32=key",
        "-getF64=key",
	"-interactive",
	static_cast<char const*>(0)
    );
}

/****************************
 ****************************
 *****  Initialization  *****
 ****************************
 ****************************/

bool Vca::Tool::getDefaultServerName (VString &rServerName) const {
    CommandCursor iCommandCursor (this);
    rServerName.setTo (iCommandCursor.firstToken ());

    return rServerName.isntEmpty ();
}


/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int argc, char *argv[]) {
    Vca::VOneAppMain_<Vca::Tool> iMain (argc, argv);
    return iMain.processEvents ();
}

// Local Variables:
// c-basic-offset: 4
// c-comment-only-line-offset: -4
// c-file-offsets: ((arglist-intro . +) (arglist-close . 0))
// End:
