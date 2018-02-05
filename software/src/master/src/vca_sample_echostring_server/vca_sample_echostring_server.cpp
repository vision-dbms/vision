/*****  Vca Sample Server  *****/

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

#include "vca_samples_iechostring.h"


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

    //  Construction
    public:
	ThisApp (Context *pContext);

    //  Destruction
    private:
	~ThisApp ();

    //  Roles
    public:
	using BaseClass::getRole;

    //--->  IEchoString Role
    private:
	Vca::VRole<ThisClass,IEchoString> m_pIEchoString;
    public:
	void getRole (IEchoString::Reference &rpRole) {
	    m_pIEchoString.getRole (rpRole);
	}

    //  Role Callbacks
    //--->  IEchoString
    public:
	void Echo (
	    IEchoString *pRole, IVReceiver<VString const&> *pClient, VString const &rString
	);

    //  Startup
    private:
	virtual bool start_() OVERRIDE;
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

VcaSamples::ThisApp::ThisApp (Context *pContext) : BaseClass (pContext), m_pIEchoString (this) {
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

void VcaSamples::ThisApp::Echo (
    IEchoString *pRole, IVReceiver<VString const&> *pClient, VString const &rString
) {
    if (pClient)
	pClient->OnData (rString);
}


/*********************
 *********************
 *****           *****
 *****  Startup  *****
 *****           *****
 *********************
 *********************/

bool VcaSamples::ThisApp::start_() {
    if (BaseClass::start_() && !offerSelf ()) {
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
    Vca::VOneAppMain_<VcaSamples::ThisApp> iMain (argc, argv);
    return iMain.processEvents ();
}
