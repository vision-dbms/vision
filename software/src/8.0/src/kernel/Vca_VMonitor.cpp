/*****  Vca VMonitor *****/

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
#include "Vca_VActivityMonitor.h"
#include "Vca_VActivitySink.h"


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VActivityPrinter  *****
 *****                         *****
 ***********************************
 ***********************************/

namespace Vca {
    class Vca_API VActivityPrinter : public VActivitySink {
	DECLARE_CONCRETE_RTTLITE (VActivityPrinter, VActivitySink);

    //  ActivityMonitor
    public:
	class ActivityMonitor : public VActivityMonitor {
	    DECLARE_CONCRETE_RTTLITE (ActivityMonitor, VActivityMonitor);

	//  Construction
	public:
	    ActivityMonitor (VActivity* pActivity);

	//  Destruction
	private:
	    ~ActivityMonitor ();

	//  Roles
	public:
	    using BaseClass::getRole;

	//  IEventSink Role
	private:
	    VRole<ThisClass,IEventSink> m_pIEventSink;
	public:
	    void getRole (IEventSink::Reference& rpRole) {
		m_pIEventSink.getRole (rpRole);
	    }

	//  IEventSink Callbacks
	public:
	    void OnEvent (
		IEventSink* pRole, IEvent* pInterface, IEvent* pProxy, VTime const& rTimestamp, VString const& rDescription
	    );
	};

    //  Construction
    public:
	VActivityPrinter (VActivityModel *pModel = 0);

    //  Destruction
    protected:
	~VActivityPrinter ();

    //  Monitoring
    public:
	void createMonitorFor (VActivity *pActivity) const;
    };
}


/***********************************
 ***********************************
 *****                         *****
 *****  Vca::VActivityPrinter  *****
 *****                         *****
 ***********************************
 ***********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivityPrinter::VActivityPrinter (VActivityModel *pModel) : BaseClass (pModel) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivityPrinter::~VActivityPrinter () {
}

/************************
 ************************
 *****  Monitoring  *****
 ************************
 ************************/

void Vca::VActivityPrinter::createMonitorFor (VActivity *pActivity) const {
//  Create an ActivityMonitor object to record and track the activity...
    ActivityMonitor::Reference const pActivityMonitor (new ActivityMonitor (pActivity));

//  ... and print some stuff to show what we got:
    IActivity::Reference pAuthority;
    pActivity->getAuthority (pAuthority);

    VString iStartTime;
    pActivity->getRemoteStartTime (iStartTime);

    VString iEndTime;
    pActivity->getRemoteEndTime (iEndTime);

//  A total hack to make multi-line descriptions easier to display:
    VString iDescription;
    pActivity->getFormattedDescription (iDescription);

    fprintf (
	stderr, "+++ %p: OnActivity (%p %s %s -)\n    %s\n\n",
	pActivity, pAuthority, iStartTime.content (), iEndTime.content (), iDescription.content ()
    );
}


/****************************************************
 ****************************************************
 *****                                          *****
 *****  Vca::VActivityPrinter::ActivityMonitor  *****
 *****                                          *****
 ****************************************************
 ****************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VActivityPrinter::ActivityMonitor::ActivityMonitor (
    VActivity* pActivity
) : BaseClass (pActivity), m_pIEventSink (this) {
    retain (); {
	requestEvents ();
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VActivityPrinter::ActivityMonitor::~ActivityMonitor () {
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VActivityPrinter::ActivityMonitor::OnEvent (
    IEventSink* pRole, IEvent* pInterface, IEvent* pProxy, VTime const& rTimestamp, VString const& rDescription
) {
//  Print some stuff to show what we got:
    VString iTimestamp;
    rTimestamp.asString (iTimestamp);

    VString iDescription;
    getFormattedDescription (iDescription);

    fprintf (
	stderr, "+++ %p: OnEvent (%s %s)\n    %s\n\n",
	activity (), iTimestamp.content (), rDescription.content (), iDescription.content ()
    );
}


/***************************
 ***************************
 *****                 *****
 *****  Vca::VMonitor  *****
 *****                 *****
 ***************************
 ***************************/

namespace Vca {
    class VMonitor : public VServerApplication, private VActivityPrinter {
	DECLARE_CONCRETE_RTTLITE (VMonitor, VServerApplication);

    //  Construction
    public:
	VMonitor (Context *pContext);

    //  Destruction
    private:
	~VMonitor ();

    //  Roles
    public:
	using BaseClass::getRole;

    //  Startup
    private:
	bool start_();
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VMonitor::VMonitor (Context *pContext) : BaseClass (pContext) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VMonitor::~VMonitor () {
}

/*********************
 *********************
 *****  Startup  *****
 *********************
 *********************/

bool Vca::VMonitor::start_() {
    if (!BaseClass::start_())
	return false;

    if (!offerSelf ()) {
	fprintf (stderr, "Usage: No address to offer object.\n");
	setExitStatus (ErrorExitValue);
    }

    return isStarting ();
}

/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int argc, char *argv[]) {
    Vca::VOneAppMain_<Vca::VMonitor> iMain (argc, argv);
    return iMain.processEvents ();
}
