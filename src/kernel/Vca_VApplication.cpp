/*****  Vca_VApplication Implementation  *****/

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

#include "Vca_VApplication.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IPassiveConnector_Gofers.h"

#include "Vca_IPassiveCallback_Gofers.h"
#include "Vca_IInfoServer.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VConnectionFactory.h"
#include "Vca_VDeviceFactory.h"
#include "Vca_VDirectory.h"
#include "Vca_VGoferInterface.h"
#include "Vca_VListenerFactory.h"
#include "Vca_VProcessFactory.h"
#include "Vca_VStatus.h"
#include "Vca_VStdPipeSource.h"

#include "Vca_VcaConnection.h"
#include "Vca_VcaGofer.h"

#include "Vca_VClassInfoHolder.h"

#include "VReceiver.h"

#include "V_VArgList.h"
#include "V_VString.h"

#include "VSimpleFile.h"
#include "VTransientServices.h"


/***************************
 ***************************
 *****      Globals    *****
 ***************************
 ***************************/

#ifdef _WIN32
#define Vca_Decl Vca_API
#else
#define Vca_Decl extern
#endif

Vca_Decl Vca::VGoferInterface<Vca::IInfoServer>::Reference g_pInfoServerGofer;

/*****************************************
 *****************************************
 *****                               *****
 *****  Vca::VApplication::Activity  *****
 *****                               *****
 *****************************************
 *****************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VApplication::Activity::Activity (VApplication *pApplication) : m_pApplication (pApplication), m_bRunning (true) {
    m_pApplication->incrementActivityCount ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VApplication::Activity::~Activity () {
    onDone ();
}

void Vca::VApplication::Activity::onSuccess () {
    if (m_bRunning.clearIfSet ()) {
	BaseClass::onActivitySuccess ();
	m_pApplication->decrementActivityCount ();
    }
}

void Vca::VApplication::Activity::onFailure (IError *pInterface, VString const &rMessage) {
    if (m_bRunning.clearIfSet ()) {
	BaseClass::onActivityFailure (pInterface, rMessage);
	m_pApplication->decrementActivityCount ();
    }
}


/************************************************
 ************************************************
 *****                                      *****
 *****  Vca::VApplication::StateTransition  *****
 *****                                      *****
 ************************************************
 ************************************************/

namespace Vca {
    class VApplication::StateTransition : public Event {
	DECLARE_CONCRETE_RTTLITE (StateTransition, Event);

    //  Construction
    public:
	StateTransition (VApplication* pApplication, RunState xNewState);

    //  Destruction
    private:
	~StateTransition ();

    //  Description
    protected:
	virtual void getDescription_(VString& rResult) const OVERRIDE;

    //  State
    private:
	RunState const m_xOldState;
	RunState const m_xNewState;
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VApplication::StateTransition::StateTransition (
    VApplication* pApplication, RunState xNewState
) : BaseClass (pApplication), m_xOldState (pApplication->makeRunState (xNewState)), m_xNewState (xNewState) {
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VApplication::StateTransition::~StateTransition () {
}

/*************************
 *************************
 *****  Description  *****
 *************************
 *************************/

void Vca::VApplication::GetRunStateName (VString& rResult, RunState xRunState) {
    if (char const* const pRunStateName = RunStateName (xRunState))
	rResult << pRunStateName;
    else {
	rResult << "[" << static_cast<unsigned int>(xRunState) << "]";
    }
}


/*******************************
 *******************************
 *****                     *****
 *****  Vca::VApplication  *****
 *****                     *****
 *******************************
 *******************************/

/***********************
 ***********************
 *****  ClassInfo  *****
 ***********************
 ***********************/

namespace Vca {
    VClassInfoHolder &VApplication::ClassInfo () {
	static VClassInfoHolder_<ThisClass> iClassInfoHolder;
	if (iClassInfoHolder.isntComplete ()) {
	    iClassInfoHolder
		.addBase (BaseClass::ClassInfo())

		.addProperty ("activityCount"                 , &ThisClass::activityCountTrackable)
		.addProperty ("description"                   , &ThisClass::description)

		.markCompleted ();
	}
	return iClassInfoHolder;
    }
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VApplication::VApplication (Context *pContext)
    : m_pContext	(pContext)
    , m_xRunState	(RunState_AwaitingStart)
    , m_pIApplication	(this)
    , m_pIPauseResume	(this)
    , m_pIStop		(this)
    , m_pIDie		(this)
{
    if (pContext)
	pContext->incrementApplicationCount ();
    retain (); {
	VTransientServices *pTSP = transientServicesProvider ();
	if (pTSP) {
	    VString const iNSName (getenv ("NSName"));
	    VTransientServices::NSEntry::Reference pNSEntry;
	    if (pTSP->getNSEntry (iNSName, pNSEntry)) {
		if (pNSEntry.isntEmpty ()) {
		    VString iEntry;
		    iEntry.setTo (pNSEntry->infoServer ());
		    if (iEntry.isntEmpty ()) {
			g_pInfoServerGofer.setTo (new Gofer::Named<IInfoServer, IDirectory>(iEntry));
		    }
		}
	    }
	}
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VApplication::~VApplication () {
    setRunStateToStopped ();
}

/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

V::VString Vca::VApplication::description () const {
    VString iResult;
    getDescription_(iResult);
    return iResult;
}


/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

/**************************
 *****  IApplication  *****
 **************************/

void Vca::VApplication::GetLogFilePath (
    IApplication *pRole, IVReceiver<VString const &> *pClient
) {
    if (pClient) {
	VString iPath;
	transientServicesProvider ()->logFilePath (iPath);
	pClient->OnData (iPath);
    }
}

void Vca::VApplication::SetLogFilePath (
    IApplication *pRole, VString const &iValue
) {
    VTransientServices *pTSP = transientServicesProvider ();
    if (pTSP) {
	pTSP->updateLogFilePath (iValue);
	pTSP->updateLogFile ();
    }
}

void Vca::VApplication::GetLogSwitch (
    IApplication *pRole, IVReceiver<bool> *pClient
) {
    if (pClient) {
	pClient->OnData (transientServicesProvider ()->logSwitch ());
    }
}

void Vca::VApplication::SetLogSwitch (
    IApplication *pRole, bool iValue
) {
     transientServicesProvider ()->updateLogSwitch (iValue);
}

void Vca::VApplication::GetLogFileSize (
    IApplication *pRole, IVReceiver<U32> *pClient
) {
    if (pClient) {
        pClient->OnData (transientServicesProvider ()->logFileSize ());
    }
}

void Vca::VApplication::SetLogFileSize (
    IApplication *pRole, U32 iValue
) {
    transientServicesProvider ()->updateLogFileSize (iValue);
}

void Vca::VApplication::GetLogFileBackups (
    IApplication *pRole, IVReceiver<U32> *pClient
) {
    if (pClient) {
        pClient->OnData (transientServicesProvider ()->logFileBackups ());
    }
}

void Vca::VApplication::SetLogFileBackups (
    IApplication *pRole, U32 iValue
) {
    transientServicesProvider ()->updateLogFileBackups (iValue);
}

void Vca::VApplication::GetStatistics (
    IApplication *pRole,  IVReceiver<VString const&> *pClient
) {
    if (pClient) {
	VString result;
	getStatistics (result);
	pClient->OnData (result);
    }
}

/*******************************
 *****  Vca::IPauseResume  *****
 *******************************/

void Vca::VApplication::Pause (IPauseResume *pRole) {
    pause ();
}

void Vca::VApplication::Resume (IPauseResume *pRole) {
    resume ();
}

/************************
 *****  Vca::IStop  *****
 ************************/

void Vca::VApplication::Stop (IStop *pRole, bool bHardStop) {
    stop (bHardStop);
}

/***********************
 *****  Vca::IDie  *****
 ***********************/

void Vca::VApplication::Die (IDie *pRole) {
    Exit (ErrorExitValue);
}


/*********************************
 *********************************
 *****  Command Line Access  *****
 *********************************
 *********************************/

bool Vca::VApplication::getDefaultServerName (VString &rServerName) const {
    return false; // no server name
}

bool Vca::VApplication::getServerName (
    VString &rServerName,
    char const *pServerTag, char const *pServerEnv,
    char const *pServerFileTag, char const *pServerFileEnv
) const {
    char const *pServerName = commandStringValue (pServerTag, pServerEnv);
    if (pServerName) {
	rServerName.setToStatic (pServerName);
	return true;
    }

    char const *pServerFileName = commandStringValue (pServerFileTag, pServerFileEnv);
    if (pServerFileName) {
	VSimpleFile iServerFile;
	if (iServerFile.GetLine (rServerName, pServerFileName))
	    return true;
    }

    return getDefaultServerName (rServerName);
}

bool Vca::VApplication::getServerName (
    VString &rServerName, char const *pServerTag, char const *pServerEnv
) const {
    char const *pServerFileTag = 0;
    VString iServerFileTag;
    if (pServerTag) {
	iServerFileTag.setTo (pServerTag);
	iServerFileTag << "File";
	pServerFileTag = iServerFileTag;
    }

    char const *pServerFileEnv = 0;
    VString iServerFileEnv;
    if (pServerEnv) {
	iServerFileEnv.setTo (pServerEnv);
	iServerFileEnv << "File";
	pServerFileEnv = iServerFileEnv;
    }
    return getServerName (
	rServerName, pServerTag, pServerEnv, pServerFileTag, pServerFileEnv
    );
}

bool Vca::VApplication::getServerName (VString &rServerName) const {
    return getServerName (
	rServerName, "server", "VisionServer", "serverFile", "VisionServerFile"
    );
}


/*********************************
 *********************************
 *****  Activity Accounting  *****
 *********************************
 *********************************/

void Vca::VApplication::incrementActivityCount() {
    if (0 == m_iActivityCount++)
	onActivityCountWasZero ();
}

void Vca::VApplication::decrementActivityCount() {
    if (m_iActivityCount.decrementIsZero ())
	onActivityCountIsZero ();
}

void Vca::VApplication::onActivityCountWasZero () {
}

void Vca::VApplication::onActivityCountIsZero () {
    stop ();
}


/********************************
 ********************************
 *****  Connection Support  *****
 ********************************
 ********************************/

bool Vca::VApplication::getServerGofer (server_gofer_t::Reference &rpServerGofer) {
    if (m_pServerGofer) {
	rpServerGofer.setTo (m_pServerGofer);
	return true;
    }

    VString iServerName;
    return getServerName (iServerName) && getServerGofer (rpServerGofer, iServerName) && setServerGofer (rpServerGofer);
}

bool Vca::VApplication::getServerGofer (server_gofer_t::Reference &rpServerGofer, VString const &rServerName) const {
    if (usingDirectory ())
	rpServerGofer.setTo (new Gofer::Named<IVUnknown,IDirectory>(rServerName));
    else {
	IPipeFactory::Reference pPipeFactory;
	supply (pPipeFactory);
	rpServerGofer.setTo (new Gofer::Named<IVUnknown,IPipeFactory>(rServerName, pPipeFactory));
    }
    return rpServerGofer.isntNil ();
}

bool Vca::VApplication::setServerGofer (server_gofer_t *pServerGofer) {
//  Cache the gofer if appropriate...
    if (m_pServerGofer.isNil () && !usingDirectory ())
	m_pServerGofer.setTo (pServerGofer);

//  Create multiple connection trunks here by requesting multiple callbacks from the server...
    size_t cExtras;
    if (getCommandValue (cExtras, "extraConnects")) {
	VGoferInterface<IPassiveServer>::Reference const pCallbackServerGofer (
	    new Gofer::Queried<IPassiveServer> (pServerGofer)
	);
	while (cExtras-- > 0) {
	    VGoferInterface<IPassiveCallback>::Reference const pCallbackGofer (
		new IPassiveCallback_Gofer::Request (
		    IPassiveConnector_Gofer::DefaultConnector (), pCallbackServerGofer, 120 * static_cast<unsigned __int64>(1000000) // == 2 minutes
		)
	    );
	    pCallbackGofer->supplyMembers (this, &ThisClass::onGoferCallbackValue, &ThisClass::onGoferCallbackError);
	}
    }

    return true;
}

void Vca::VApplication::onGoferCallbackValue (IPassiveCallback *pCallback) {
}

void Vca::VApplication::onGoferCallbackError (IError*, VString const &rMessage) {
}

void Vca::VApplication::onConnectError (IError *pError, VString const &rMessage) {
    fprintf (stderr, ">>> OnConnect: %s\n", rMessage.content ());
    stop ();
}

bool Vca::VApplication::usingDirectory () const {
    return commandSwitchValue ("directory");
}


/***********************************
 ***********************************
 *****  Device Factory Access  *****
 ***********************************
 ***********************************/

bool Vca::VApplication::supply (IConnectionFactory::Reference &rpInterface) const {
    return VConnectionFactory::Supply (rpInterface);
}

bool Vca::VApplication::supply (IListenerFactory::Reference &rpInterface) const {
    return VListenerFactory::Supply (rpInterface);
}

bool Vca::VApplication::supply (IPipeFactory::Reference &rpInterface) const {
    return VConnectionFactory::Supply (rpInterface);
}

bool Vca::VApplication::supply (IProcessFactory::Reference &rpInterface) const {
    return VProcessFactory::Supply (rpInterface);
}

bool Vca::VApplication::supply (IStdPipeSource::Reference &rpInterface) const {
    return VStdPipeSource::Supply (rpInterface);
}


/************************************
 ************************************
 *****  Standard Device Access  *****
 ************************************
 ************************************/

void Vca::VApplication::getStandardInput () {
    VkStatus iStatus; VBSProducer::Reference pBSToHere;
    if (VDeviceFactory::SupplyStdToHere (iStatus, pBSToHere))
	onStandardInput (pBSToHere);
    else {
	onStandardChannelAccessStatus (iStatus);
    }
}

void Vca::VApplication::getStandardOutput () {
    VkStatus iStatus; VBSConsumer::Reference pBSToPeer;
    if (VDeviceFactory::SupplyStdToPeer (iStatus, pBSToPeer))
	onStandardOutput (pBSToPeer);
    else {
	onStandardChannelAccessStatus (iStatus);
    }
}

void Vca::VApplication::getStandardError () {
    VkStatus iStatus; VBSConsumer::Reference pErrToPeer;
    if (VDeviceFactory::SupplyErrToPeer (iStatus, pErrToPeer))
	onStandardError (pErrToPeer);
    else {
	onStandardChannelAccessStatus (iStatus);
    }
}

void Vca::VApplication::getStandardChannels2 () {
    VkStatus iStatus; VBSProducer::Reference pBSToHere; VBSConsumer::Reference pBSToPeer;
    if (VDeviceFactory::Supply (iStatus, pBSToHere, pBSToPeer))
	onStandardChannels (pBSToHere, pBSToPeer);
    else {
	onStandardChannelAccessStatus (iStatus);
    }
}

void Vca::VApplication::getStandardChannels3 () {
    VkStatus iStatus; VBSProducer::Reference pBSToHere; VBSConsumer::Reference pBSToPeer, pErrToPeer;
    if (VDeviceFactory::Supply (iStatus, pBSToHere, pBSToPeer, pErrToPeer))
	onStandardChannels (pBSToHere, pBSToPeer, pErrToPeer);
    else {
	onStandardChannelAccessStatus (iStatus);
    }
}

void Vca::VApplication::onStandardInput (VBSProducer *pI) {
}

void Vca::VApplication::onStandardOutput (VBSConsumer *pO) {
}

void Vca::VApplication::onStandardError (VBSConsumer *pE) {
}

void Vca::VApplication::onStandardChannels (VBSProducer *pI, VBSConsumer *pO) {
}

void Vca::VApplication::onStandardChannels (VBSProducer *pI, VBSConsumer *pO, VBSConsumer *pE) {
}

void Vca::VApplication::onStandardChannelAccessError (IError *pError, VString const &rMessage) {
}

void Vca::VApplication::onStandardChannelAccessStatus (VkStatus const& rStatus) {
    VString iMessage;
    rStatus.getDescription (iMessage);
    onStandardChannelAccessError (0, iMessage);
}


/**********************************
 **********************************
 *****  Activity Description  *****
 **********************************
 **********************************/

void Vca::VApplication::getDescription_(VString& rResult) const {
    rResult << arg0();
    for (unsigned int xArg = 1; xArg < argc(); xArg++) {
	rResult << " " << argv()[xArg];
    }
}


/***********************************
 ***********************************
 *****  Run State Description  *****
 ***********************************
 ***********************************/

char const* Vca::VApplication::RunStateName (RunState xRunState) {
    switch (xRunState) {
    case RunState_AwaitingStart:
	return "AwaitingStart";
    case RunState_Starting:
	return "Starting";
    case RunState_Running:
	return "Running";
    case RunState_Pausing:
	return "Pausing";
    case RunState_Paused:
	return "Paused";
    case RunState_Resuming:
	return "Resuming";
    case RunState_Stopping:
	return "Stopping";
    case RunState_AwaitingStop:
	return "AwaitingStop";
    case RunState_Stopped:
	return "Stopped";
    }
    return 0;
}

void Vca::VApplication::StateTransition::getDescription_(VString& rResult) const {
    rResult << "Application state transition from ";
    GetRunStateName (rResult, m_xOldState);
    rResult << " to ";
    GetRunStateName (rResult, m_xNewState);
}

/*******************************
 *******************************
 *****  Run State Control  *****
 *******************************
 *******************************/

bool Vca::VApplication::start () {
    if (setRunStateIf (RunState_AwaitingStart, RunState_Starting)) {
	incrementActivityCount ();
	onStart ();
	start_();
	decrementActivityCount ();
    }
    return setRunStateIf (RunState_Starting, RunState_Running);
}

bool Vca::VApplication::stop (bool bHardStop) {
    if (bHardStop && isntStopped ()) {
	m_xRunState = RunState_Stopping; // setRunStateIf (m_xRunState, RunState_Stopping); ???
	stop_(true);
    } else if (isntStopping () && isntAwaitingStop () && isntStopped ()) {
	m_xRunState = RunState_Stopping; // setRunStateIf (m_xRunState, RunState_Stopping); ???
	stop_(bHardStop);
    }
    return setRunStateIf (RunState_Stopping, RunState_Stopped);
}

bool Vca::VApplication::pause () {
    if (setRunStateIf (RunState_Running, RunState_Pausing))
	pause_();
    return setRunStateIf (RunState_Pausing, RunState_Paused);
}

bool Vca::VApplication::resume () {
    if (setRunStateIf (RunState_Paused, RunState_Resuming))
	resume_();
    return setRunStateIf (RunState_Resuming, RunState_Running);
}


/**********************************
 **********************************
 *****  Run State Transition  *****
 **********************************
 **********************************/

Vca::VApplication::RunState Vca::VApplication::makeRunState (RunState xNewState) {
    RunState const xOldState = m_xRunState;
    m_xRunState = xNewState;
    return xOldState;
}

bool Vca::VApplication::setRunStateIf (RunState xOldState, RunState xNewState) {
    if (m_xRunState != xOldState)
	return false;

    if (xOldState != xNewState) {
        onRunStateChange_(xOldState, xNewState);
    }
    return true;
}

bool Vca::VApplication::setRunStateToStopped () {
    return isntStopped () && setRunStateIf (m_xRunState, RunState_Stopped);
}

void Vca::VApplication::onRunStateChange_(RunState xOldState, RunState xNewState) {
    onEvent (new StateTransition (this, xNewState));
    if (m_pRunStateReceiver)
        m_pRunStateReceiver->OnData (m_xRunState);
    if (isStopped ()) {
        onSuccess ();
        if (m_pContext)
            m_pContext->decrementApplicationCount ();
    }
}

bool Vca::VApplication::start_() {
    return isStarting ();
}

bool Vca::VApplication::stop_(bool bHardStop) {
    return isStopping (bHardStop);
}

bool Vca::VApplication::pause_() {
    return isPausing ();
}

bool Vca::VApplication::resume_() {
    return isResuming ();
}

void Vca::VApplication::onRunStateChangeNotify (IRunStateReceiver *pReceiver) {
    if (m_pRunStateReceiver)
	m_pRunStateReceiver->OnEnd ();
    m_pRunStateReceiver.setTo (pReceiver);
    if (m_pRunStateReceiver)
	m_pRunStateReceiver->OnData (m_xRunState);
}


/*****************************
 *****************************
 *****  Usage Reporting  *****
 *****************************
 *****************************/

void Vca::VApplication::displayUsagePattern (char const *pPattern) const {
    fprintf (stderr, "\nUsage: %s %s\n", arg0 (), pPattern);
}

void __cdecl Vca::VApplication::displayUsagePatterns (char const *pPattern, ... ) const {
    V_VARGLIST (pArguments, pPattern);

    fprintf (stderr, "\nUsage:\n");
    while (pPattern) {
	fprintf (stderr, "   %s %s\n", arg0 (), pPattern);
	pPattern = pArguments.arg<char const*>();
    }
}

void __cdecl Vca::VApplication::displayOptions (char const *pOption, ... ) {
    V_VARGLIST (pArguments, pOption);

    while (pOption) {
	fprintf (stderr, "   %s\n", pOption);
	pOption = pArguments.arg<char const*>();
    }
}

void Vca::VApplication::displayStandardOptions () {
    VString iMsg;
    iMsg << "Standard Vca Application Options:";	

    displayOptions (
	iMsg.content (),
	"      Value Options:",
	"          -logFilePath=<path>",
	"          -logFileSize=<size(mb)>",
	"          -logFileBackups=<count>",
	"      Control Options:",
	"          -logSwitchOn",
	"      Trace Options:",
	"          -traceObjects",
	"          -traceProxyGC",
	"          -traceTransports",
#ifdef __linux__
        "          -traceSSH",
#endif
	"      Tuning Options:",
	"          -vcaBufferSize=#",
	"          -vcaFifoTransportPolicy",
//	"          -vcaProxyTransferSize=#,
	static_cast<char const*>(0)
    );
}


/************************
 ************************
 *****  Statistics  *****
 ************************
 ************************/

void Vca::VApplication::getStatistics (VString &rResult) const {
    VTransientServices *pTSP = transientServicesProvider ();
    if (pTSP) {
        rResult.append ("\nApplication Statistics: ");
	rResult.append ("\n   Log File Switch: ");
	pTSP->logSwitch () ? rResult.append (" On") : rResult.append ("Off");
    
        rResult.append ("\n   Log File Path: ");
	VString rPath;
	pTSP->logFilePath (rPath);
	rResult.append (rPath);

	rResult.append ("\n   Log File Size: ");
	rResult << pTSP->logFileSize ();
	rResult.append ("(Mb)");

        rResult.append ("\n   Log File Backups: ");
	rResult << pTSP->logFileBackups ();
        rResult.append ("\n");
    }
}

/*******************
 *******************
 *****  Query  *****
 *******************
 *******************/

bool Vca::VApplication::isBackground () const {
    return commandSwitchValue ("background");
}

/*************************************
 *************************************
 *****  Tracking and Trackables  *****
 *************************************
 *************************************/

Vca::VTrackable_count_t const &Vca::VApplication::activityCountTrackable () {
    return m_iActivityCount.trackable ();
}
