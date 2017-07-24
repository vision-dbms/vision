/*****  VsaPrompt Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/****************************
 ***** Shutdown System  *****
 ****************************/

#define V_USING_FAST_SHUTDOWN_HEADERS 1
#include "VConfig.h"

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VsaPrompt.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VOneAppMain_.h"

#include "VkStatus.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VDeviceFactory.h"
#include "Vca_VNotifier.h"

#include "Vsa_VPrompt.h"


Vca::VTransientServicesForNotification g_iTransientServicesForNotification;

/*************************************
 *************************************
 *****                           *****
 *****  Vca::VPromptApplication  *****
 *****                           *****
 *************************************
 *************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vsa::VPromptApplication::VPromptApplication (Context *pContext) : BaseClass (pContext) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vsa::VPromptApplication::~VPromptApplication () {
}


/****************************
 ****************************
 *****  Initialization  *****
 ****************************
 ****************************/

bool Vsa::VPromptApplication::getDefaultServerName (VString &rServerName) const {
    CommandCursor iCommandCursor (this);
    rServerName.setTo (iCommandCursor.firstToken ());

    return rServerName.isntEmpty ();
}

bool Vsa::VPromptApplication::start_() {
    if (!BaseClass::start_())
	return false;

//  Before actually creating a 'VPrompt' activity, gofer an evaluator to ensure it can be gotten...
    if (getInterfaceGofer (m_pEvaluatorGofer)) {
	incrementActivityCount ();
	m_pEvaluatorGofer->supplyMembers (this, &ThisClass::onGoferValue, &ThisClass::onGoferError);
    } else {
	displayUsagePatterns (
    	    "<value-options> <tag-options> <trace-options>",
	    static_cast<char const*>(0)
	);
	displayOptions (
	    "  Value Options:",
	    "      <poolServerPort> |",
	    "      -server=<poolServerPort> |", 
	    "      -serverFile=<poolServer-file>",
	    static_cast<char const*>(0)
	);
	displayOptions (
	    "  [Control] Tag Options:",
	    "      -directory (interpret supplied value as a directory name",
	    "                  rather than a connection specification)",
	    "      -anyData (Any worker can be used to answer the queries)",
	    "      -E (Use Extended prompt)",
	    static_cast<char const*>(0)
	);
	displayStandardOptions ();
	setExitStatus (ErrorExitValue);
    }

    return isStarting ();
}

void Vsa::VPromptApplication::onGoferValue (IEvaluator *pEvaluator) {
    VkStatus iStatus; Vca::VBSProducer::Reference pBSToHere; Vca::VBSConsumer::Reference pBSToPeer;
    if (pEvaluator && Vca::VDeviceFactory::Supply (iStatus, pBSToHere, pBSToPeer)) {
    //  Create the prompt's implementation object...
	VPrompt::Reference const pPromptImplementation (
	    new VPrompt (
		this, m_pEvaluatorGofer, commandSwitchValue("reconnect") ? static_cast<IEvaluator*>(0) : pEvaluator, pBSToPeer
	    )
	);

    //  ... and start the input flowing:
	VPrompt::LineGrabber::Reference const pInputProvider (new VPrompt::LineGrabber (pBSToHere, pPromptImplementation));
    }
    else {
      display ("ERROR: Server doesnt support IEvaluator interface\n");
    }
    decrementActivityCount ();
}


/**************************
 **************************
 *****                *****
 *****  Main Program  *****
 *****                *****
 **************************
 **************************/

int main (int argc, char *argv[]) {
    Vca::VOneAppMain_<Vsa::VPromptApplication> iMain (argc, argv);

#if defined(__VMS)
    if (IsntNil(getenv("EnableVPromptLog"))) { 
	ON_BLOCK_EXIT(invoke_fast_shutdown, 0);
	return iMain.processEvents ();
    }
    else 
#endif
    return iMain.processEvents ();
}
