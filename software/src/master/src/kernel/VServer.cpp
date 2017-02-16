/*****  VServer Implementation  *****/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VServer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VOneAppMain_.h"

#include "Vca_IDirectory.h"

#include "Vca_VActivitySink.h"
#include "Vca_VDirectory.h"


/**************************
 **************************
 *****                *****
 *****  Vca::VServer  *****
 *****                *****
 **************************
 **************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VServer::VServer (Context *pContext) : BaseClass (pContext), VActivityRelay (this) {
//  If requested, offer one of my directory entries, ...
    if (char const* pOfferName = commandStringValue ("offerEntry")) {
	aggregate (new Gofer::Named<IVUnknown,IDirectory>(pOfferName));
    }

//  If requested, offer a/my directory, ...
    if (commandSwitchValue ("offerDirectory")) {
	aggregate (new VDirectory ());
    } else if (commandSwitchValue ("offerDirectoryEntries")) {
	IDirectory::Reference pRootDirectory;
	GetRootDirectory (pRootDirectory);
	aggregate (pRootDirectory);
    }
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VServer::~VServer () {
}

/***********************
 ***********************
 *****  Execution  *****
 ***********************
 ***********************/

bool Vca::VServer::start_() {
    if (!BaseClass::start_())
	return false;

    if (!offerSelf ()) {
	displayOptions (
	    "Options:",
	    "  -offerDirectory                    ... expose an empty directory",
	    "  -offerDirectoryEntries             ... expose the session directory",
	    "  -offerEntry=directory-entry-name   ... expose a session directory entry",
	    static_cast<char const*>(0)
	);
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
    Vca::VOneAppMain_<Vca::VServer> iMain (argc, argv);
    return iMain.processEvents ();
}
