/*****  Vca Sample Client  *****/

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

#include "vca_samples_iechostring.h"


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

    //  Input Processor
    public:
	class InputProcessor;

    //  Construction
    public:
	ThisApp (Context *pContext);

    //  Destruction
    private:
	~ThisApp ();

    //  Roles
    public:
	using BaseClass::getRole;

    //--->  IVReceiver<VString const&>
    private: //  ... form for templated interfaces
	Vca::VRole<ThisClass,IVReceiver<VString const&> > m_pIStringReceiver;
    public:
	void getRole (IVReceiver<VString const&>::Reference &rpRole) {
	    m_pIStringReceiver.getRole (rpRole);
	}

    //  Role Callbacks
    //--->  IVReceiver<VString const&>
    public:
	void OnData (IVReceiver<VString const&> *pRole, VString const &rString);
	void OnError (IVReceiver<VString const&> *pRole, Vca::IError *pError, VString const &rMessage);
	using BaseClass::OnError;

    //  Startup
    private:
	bool start_();

	void onServer (IEchoString *pEchoServer);

    //  Reply Management
    private:
	void onReply ();

    //  State
    private:
	unsigned int m_cUnansweredRequests;
    };
}


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

VcaSamples::ThisApp::ThisApp (Context *pContext)
    : BaseClass (pContext), m_cUnansweredRequests (0), m_pIStringReceiver(this)
{
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

void VcaSamples::ThisApp::OnData (IVReceiver<VString const&> *pRole, VString const &rString) {
    printf ("+++ Got %s\n", rString.content ());
    onReply ();
}

void VcaSamples::ThisApp::OnError (IVReceiver<VString const&> *pRole, Vca::IError *pError, VString const &rMessage) {
    printf ("+++ Got Error: %s\n", rMessage.content ());
    onReply ();
}

/******************************
 ******************************
 *****  Reply Management  *****
 ******************************
 ******************************/

void VcaSamples::ThisApp::onReply () {
    if (0 == --m_cUnansweredRequests)
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

    if (!getServerInterface (this, &ThisClass::onServer)) {
	fprintf (stderr, "No Server To Contact");
    }

    return isStarting ();
}

void VcaSamples::ThisApp::onServer (IEchoString *pEchoServer) {
    if (pEchoServer) {
	IVReceiver<VString const&>::Reference pRole;
	getRole (pRole);

	CommandCursor iCommandLineCursor (this);
	char const *pToken = iCommandLineCursor.nextToken ();
	if (!pToken)
	    stop ();
	else do {
	    m_cUnansweredRequests++;
	    pEchoServer->Echo (pRole, pToken);
	    pToken = iCommandLineCursor.nextToken ();
	} while (pToken);
    } else {
	fprintf (stderr, "+++ No Server At Address\n", arg0 ());
	setExitStatus (ErrorExitValue);
	stop ();
    }
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
