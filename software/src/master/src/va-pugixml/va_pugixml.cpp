/*****  Vision<->PugiXML Server  *****/

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

#include "Vxa_VServerApplication.h"

#include "va_pugixml_root.h"


/**********************************
 **********************************
 *****                        *****
 *****  VA::PugiXML::ThisApp  *****
 *****                        *****
 **********************************
 **********************************/

namespace VA {
    namespace PugiXML {
	class ThisApp : public Vxa::VServerApplication {
	    DECLARE_CONCRETE_RTTLITE (ThisApp, Vxa::VServerApplication);

	//  Construction
	public:
	    ThisApp (Context *pContext);

	//  Destruction
	private:
	    ~ThisApp ();

	//  Root Offering
	private:
	    void getRootObject () {
		offer (Vxa::Export (new Root ()));
	    }
	};
    }
}


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTTLITE (VA::PugiXML::ThisApp);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VA::PugiXML::ThisApp::ThisApp (Context *pContext) : BaseClass (pContext) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VA::PugiXML::ThisApp::~ThisApp () {
}



/**************************
 **************************
 *****  Main Program  *****
 **************************
 **************************/

int main (int argc, char *argv[]) {
    Vca::VOneAppMain_<VA::PugiXML::ThisApp> iMain (argc, argv);
    return iMain.processEvents ();
}
