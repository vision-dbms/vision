/*****  Main Program Shell  *****/

/***************************
 *****  Facility Name  *****
 ***************************/

#define FacilityName BatchvisionMain

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"
#include "VkMemory.h"

/******************
 *****  Self  *****
 ******************/

#include "batchvision.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VOneAppMain_.h"
#include "Vca_VClassInfoHolder.h"

#include "gopt.h"
#include "m.h"

#include "verr.h"
#include "vfac.h"
#include "vfault.h"
#include "vstdio.h"
#include "vsignal.h"
#include "vunwind.h"
#include "vutil.h"

#include "IOMStream.h"

#include "VComputationScheduler.h"

#include "Vca_IDirectory.h"
#include "Vca_IStdPipeSource.h"
#include "Vca_IStdPipeSourceClient.h"

#include "Vca_VBSConsumer.h"
#include "Vca_VBSProducer.h"

#include "Vca_VCohort.h"
#include "Vca_VDirectory.h"
#include "Vca_VLoopbackDeviceFactory.h"

#include "Vsa_IEvaluatorClient.h"

#include "cam.h"


/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

Batchvision::Reference Batchvision::g_pTheInstance;
VTransientServicesForBatchvision ThisProcess;


/*******************************
 *******************************
 *****                     *****
 *****  Batchvision::Main  *****
 *****                     *****
 *******************************
 *******************************/

class Batchvision::Main : public Vca::VOneAppMain_<Batchvision> {
    DECLARE_FAMILY_MEMBERS (Main, Vca::VOneAppMain_<Batchvision>);

//  Construction
public:
    Main (int argc, argv_t argv);

//  Destruction
public:
    ~Main ();

//  Exit Value
public:
    int getExitValue () const;

//  Event Loop Override
private:
    void eventLoopImplementation_();
};

Batchvision::Main::Main (int argc, argv_t argv) : BaseClass (argc, argv) {
//  Initialize the GOPT options manager too...
    GOPT_AcquireOptions (argc, argv);
    Vca::VDirectory::DisableTTL ();
}

Batchvision::Main::~Main () {
    g_pTheInstance.clear ();
}

int Batchvision::Main::getExitValue () const {
    return theApp () ? theApp ()->getExitValue () : ErrorExitValue;
}

void Batchvision::Main::eventLoopImplementation_() {
    g_pTheInstance->DoEverything ();
}


/**********************************
 **********************************
 *****                        *****
 *****  Batchvision::Plumber  *****
 *****                        *****
 **********************************
 **********************************/

class Batchvision::Plumber : public Vca::VRolePlayer {
    DECLARE_CONCRETE_RTTLITE (Plumber, Vca::VRolePlayer);

//  Aliases
public:
    typedef Vca::IStdPipeSourceClient	IStdPipeSourceClient;

    typedef Vca::VBSConsumer		VBSConsumer;
    typedef Vca::VBSProducer		VBSProducer;

//  Construction
public:
    Plumber (Batchvision *pBatchvision, IStdPipeSource *pStdPipeSource);

//  Destruction
private:
    ~Plumber ();

//  Base Roles
public:
    using BaseClass::getRole;

//  IStdPipeSourceClient Role
private:
    Vca::VRole<ThisClass,Vca::IStdPipeSourceClient> m_pIStdPipeSourceClient;
public:
    /**
     * Retrieves the IStdPipeSourceClient role for this Batchvision::Plumber.
     *
     * @param[out] rpRole the reference that will be used to return the IStdPipeSourceClient for this Batchvision::Plumber.
     */
    void getRole (IStdPipeSourceClient::Reference &rpRole) {
	m_pIStdPipeSourceClient.getRole (rpRole);
    }

//  IStdPipeSourceClient Methods
public:
    void OnData (
	IStdPipeSourceClient*	pRole,
	VBSProducer*		pStdBSToHere,
	VBSConsumer*		pStdBSToPeer,
	VBSConsumer*		pErrBSToPeer
    );

//  State
private:
    Batchvision::Reference const m_pBatchvision;
};


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Batchvision::Plumber::Plumber (
    Batchvision *pBatchvision, IStdPipeSource *pStdPipeSource
) : m_pBatchvision (pBatchvision), m_pIStdPipeSourceClient (this) {
    retain (); {
	IStdPipeSourceClient::Reference pRole;
	getRole (pRole);
	pStdPipeSource->Supply (pRole);
    } untain ();
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Batchvision::Plumber::~Plumber () {
}

/*******************
 *******************
 *****  Roles  *****
 *******************
 *******************/

void Batchvision::Plumber::OnData (
    IStdPipeSourceClient *pRole, VBSProducer *pStdBSToHere, VBSConsumer *pStdBSToPeer, VBSConsumer *pErrBSToPeer
) {
    IOMDriver::Options iOptions;
    iOptions.SetPromptFormatTo (
	GOPT_GetSwitchOption ("FromEditor") > 0 
	? IOMPromptFormat_Editor : IOMPromptFormat_Normal
    );

    IOMStream::Reference pDriver (
	new IOMStream (iOptions, pStdBSToHere, pStdBSToPeer)
    );
    pDriver->SetStartupExpressionTo (GOPT_GetValueOption ("StartupExpression"));
    if (pErrBSToPeer) {
	pDriver->SetErrorOutputDriver (
	    new IOMStream (iOptions, NilOf (VBSProducer*), pErrBSToPeer)
	);
    }
    pDriver->EnableHandler ();
}


/*************************
 *************************
 *****               *****
 *****  Batchvision  *****
 *****               *****
 *************************
 *************************/

/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (Batchvision);


/***********************
 ***********************
 *****  ClassInfo  *****
 ***********************
 ***********************/

namespace {
    VString GetFaultDumps () {
        VString iResult;
	FAULT_DumpFaults (iResult);
	return iResult;
    }
}

Vca::VClassInfoHolder &Batchvision::ClassInfo () {
    static Vca::VClassInfoHolder_<ThisClass> iClassInfoHolder;
    if (iClassInfoHolder.isntComplete ()) {
	iClassInfoHolder
	    .addBase (BaseClass::ClassInfo ())

	    .addProperty ("faultsDump", &GetFaultDumps)

	    .markCompleted ();
    }
    return iClassInfoHolder;
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Batchvision::Batchvision (
    Context *pContext
) : BaseClass (pContext), m_pIDirectoryEntrySink (this), m_pIEvaluator (this) {
    setDebuggerEnabledTo (GOPT_GetSwitchOption ("Developer") > 0);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Batchvision::~Batchvision () {
#if defined (sun)
    Vca::VCohort::CallExitHandler ();
#endif
}


/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Batchvision::OnData (IDirectoryEntrySink *pRole, IDirectoryEntry* pSubscription) {
}

void Batchvision::EvaluateExpression (
    IEvaluator *pRole, IEvaluatorClient *pClient, VString const &rPath, VString const &rExpression
) {
    if (evaluatorMaterialized ())
	m_pEvaluatorImplementation->EvaluateExpression (pClient, rPath, rExpression);
    else if (pClient)
	pClient->OnError (0, "Batchvision Evaluator Materialization Error");
}

void Batchvision::EvaluateURL (
    IEvaluator *pRole, IEvaluatorClient *pClient, VString const &rPath, VString const &rQuery, VString const &rEnvironment
) {
    if (evaluatorMaterialized ())
	m_pEvaluatorImplementation->EvaluateURL (pClient, rPath, rQuery, rEnvironment);
    else if (pClient)
	pClient->OnError (0, "Batchvision Evaluator Materialization Error");
}

void Batchvision::EvaluateExpression_Ex (
	IEvaluator *pRole, IEvaluatorClient *pClient, VString const &rPath, VString const &rExpression, VString const &rID, VString const& rCID
) {
    CAM_OPERATION(co) << "message" << "Batchvision::EvaluateExpression_Ex() receiving query ID: %qid" << rID;
    co.stitchParent(rID.content(), rCID.content());
    EvaluateExpression(pRole, pClient, rPath, rExpression);
}
void Batchvision::EvaluateURL_Ex (
    IEvaluator *pRole, IEvaluatorClient *pClient, VString const &rPath, VString const &rQuery, VString const &rEnvironment, VString const &rID, VString const& rCID
) {
    CAM_OPERATION(co) << "message" << "Batchvision::EvaluateURL_Ex() receiving query ID: %qid" << rID;
    co.stitchParent(rID.content(), rCID.content());
    EvaluateURL(pRole, pClient, rPath, rQuery, rEnvironment);
}




/***************************************
 ***************************************
 *****  Evaluator Materialization  *****
 ***************************************
 ***************************************/

bool Batchvision::evaluatorMaterialized () {
    Vca::VLoopbackDeviceFactory::Reference pLDF;
    VBSConsumer::Reference pBSFromPump; VBSProducer::Reference pBSToDriver;
    VBSConsumer::Reference pBSFromDriver; VBSProducer::Reference pBSToPump;
    if (m_pEvaluatorImplementation.isNil () &&
	Vca::VLoopbackDeviceFactory::Supply (pLDF) &&
	pLDF->supply (pBSToDriver, pBSFromPump) &&
	pLDF->supply (pBSToPump, pBSFromDriver)
    ) {
//  Must reference a data item from Vsa before Vsa static constructors are called (Sun feature?)
	VTypeInfo::Reference pTI (IEvaluator::typeInfo ());

    //  Create an evaluator pump from one end of the loopback streams just created, ...
	VEvaluatorPump::PumpSettings::Reference const pEvaluatorSettings (
	    new VEvaluatorPump::PumpSettings ()
	);
	m_pEvaluator.setTo (
	    new VEvaluatorPump (pEvaluatorSettings, pBSFromPump, pBSToPump)
	);
	m_pEvaluator->getRole (m_pEvaluatorImplementation);

    //  ... and create and start an IOMStream from the other end of the loopback:
	IOMDriver::Options iOptions;
	iOptions.SetEvaluatorTo (m_pEvaluator);

	IOMStream::Reference const pDriver (
	    new IOMStream (iOptions, pBSToDriver, pBSFromDriver)
	);
	pDriver->SetStartupExpressionTo (GOPT_GetValueOption ("StartupExpression"));
	pDriver->EnableHandler ();
    }
    return m_pEvaluatorImplementation.isntNil ();
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void Batchvision::doConnect (IStdPipeSource *pStdPipeSource) {
    if (pStdPipeSource)
	Plumber::Reference pPlumber (new Plumber (this, pStdPipeSource));
}

bool Batchvision::start_() {
    if (!BaseClass::start_() || !g_pTheInstance.setIfNil (this))
	return false;

    if (!offerSelf ()) {
	incrementActivityCount ();
    //  If this process isn't speaking Vca, display a V> prompt...
	if (!getServerInterface (this, &ThisClass::doConnect)) {
	    IStdPipeSource::Reference pStdPipeSource;
	    supply (pStdPipeSource);
	    doConnect (pStdPipeSource);
	}
    } else if (!evaluatorMaterialized ()) {
	fprintf (stderr, ">>> Server Initialization Failed\n");
	setExitStatus (ErrorExitValue);
    }

    return isStarting ();
}

bool Batchvision::stop_(bool bHardstop) {
    if (!BaseClass::stop_(bHardstop))
	return false;

    if (bHardstop) {
	StopEverything ();
    }

    return isStopping (bHardstop);
}

void Batchvision::RestartProcess () {
    if (g_pTheInstance)
	g_pTheInstance->restartProcess ();
}

void Batchvision::restartProcess () {
#   if defined (_WIN32)
    execvp (arg0(), (char const* const*)argv());
#   else

#   if defined(__hp9000s800) || defined (__hp9000s700)
    M_ReclaimAllSegments ();
#   endif
    VkMemory::StopGRM ();

    execvp (arg0(), argv());
#   endif

    char const *pSystemErrorMessage = strerror (errno);
    ERR_SignalFault (
	EC__SystemFault, UTIL_FormatMessage (
	    "RestartSession failed: '%s'",
	    pSystemErrorMessage ? pSystemErrorMessage : UTIL_FormatMessage (
		"Error#%d", errno
	    )
	)
    );
}

void Batchvision::setExitValue (int value) {
    setExitStatus (value);
}

int Batchvision::getExitValue () {
    return FAULT_SessionErrorCount > 0 ? ErrorExitValue : getExitStatus ();
}

void Batchvision::registerREPController (VReadEvalPrintController *pREP) {
    if (g_pTheInstance)
	g_pTheInstance->setREPController (pREP);
}

void Batchvision::setREPController (VReadEvalPrintController *pREP) {
    m_pREP.setIfNil (pREP);
}

void Batchvision::completeUsageLog () {
    if (g_pTheInstance)
	g_pTheInstance->logUsage ();
}

void Batchvision::logUsage () {
    if (m_pREP)
	m_pREP->logUsage ();
}

void Batchvision::ToggleDebuggerEnabled () {
    if (g_pTheInstance)
	g_pTheInstance->toggleDebuggerEnabled ();
}


/********************************
 ********************************
 *****  Facility Management *****
 ********************************
 ********************************/

/***********************************
 *****  Facility Declarations  *****
 ***********************************/

FAC_DeclareFacility (VDEBUG);
FAC_DeclareFacility (ENVIR);
FAC_DeclareFacility (ERR);
FAC_DeclareFacility (ERRDEF);
FAC_DeclareFacility (IOM);
FAC_DeclareFacility (M);
FAC_DeclareFacility (MAGIC);
FAC_DeclareFacility (PRIMFNS);
FAC_DeclareFacility (RSLANG);
FAC_DeclareFacility (RTYPE);
FAC_DeclareFacility (SELECTOR);
FAC_DeclareFacility (VMACHINE);
FAC_DeclareFacility (rtREFUV);
FAC_DeclareFacility (rtVECTOR);


/*************************************
 *****  Facility Initialization  *****
 *************************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to initialize at system startup those facilities
 *****  requiring initialization services.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void InitializeSystemFacilities () {
//  Start the stream manager (enables Winsock calls during bootstrap), ...
    VString iMessage;
    if (!Vca::Start (&iMessage)) ERR_SignalFault (
	EC__UsageError, UTIL_FormatMessage (
	    "Stream Manager Initialization: %s", iMessage.content ()
	)
    );

    FAC_Facility failedFacility;
    FAC_CompletionCode completionCode = FAC_InitializeFacilities (
	&failedFacility,

/*---------------------------------------------------------------------------
 * List the facilities requiring special processing at system startup here...
 * (Order may be important!!!)
 *---------------------------------------------------------------------------
 */
	ERRDEF,
	ERR,
	RTYPE,
	M,
	MAGIC,
	SELECTOR,
	RSLANG,
	ENVIR,
	VMACHINE,
	PRIMFNS,   /* must be after vmachine */
	VDEBUG,
	IOM,
	rtREFUV,
	rtVECTOR,

/*****  'NilOf (FAC_Facility)' must ALWAYS be the last entry in this list  *****/
	NilOf (FAC_Facility)
    );

//  If the initialization failed, abort this session...
    if (FAC_RequestSucceeded != completionCode) {
        IO_fprintf (
	    stderr,
	    "%s: %s Error Initializing Facility: %s\n",
	    GOPT_ProgramName (),
	    FAC_CompletionCodeName (completionCode),
	    FAC_FacilityDescription (failedFacility)
	);
	UNWIND_ThrowException ();
    }
}


/*******************************
 *****  Facility Shutdown  *****
 *******************************/

/*---------------------------------------------------------------------------
 *****  Internal routine to cleanup at system shutdown those facilities
 *****  requiring cleanup services.
 *
 *  Arguments:
 *	NONE
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef void ShutDownSystemFacilities () {
    FAC_Facility failedFacility;
    FAC_CompletionCode completionCode = FAC_ShutdownFacilities (
	&failedFacility,

/*---------------------------------------------------------------------------
 * List the facilities requiring special processing at system shutdown here...
 * (Order may be important!!!)
 *---------------------------------------------------------------------------
 */
	IOM,

/*****  'NilOf (FAC_Facility)' must ALWAYS be the last entry in this list  *****/
	NilOf (FAC_Facility)
    );

//  ... report any shutdown failures, ...
    if (FAC_RequestSucceeded != completionCode) {
        IO_fprintf (
	    stderr,
	    "%s: %s Error Shutting Down Facility: %s\n",
	    GOPT_ProgramName (),
	    FAC_CompletionCodeName (completionCode),
	    FAC_FacilityDescription (failedFacility)
	);
	UNWIND_ThrowException ();
    }

//  ... and stop the stream manager:
    VString iMessage;
    if (!Vca::Stop (&iMessage)) ERR_SignalFault (
	EC__UsageError, UTIL_FormatMessage (
	    "Stream Manager Shutdown: %s", iMessage.content ()
	)
    );
}


/****************************
 ****************************
 *****  Error Handling  *****
 ****************************
 ****************************/

/***************************
 *  Default Error Handler  *
 ***************************/

/*---------------------------------------------------------------------------
 *****  The default read-eval-print loop error handler.
 *
 *  Arguments:
 *	errorDescription	- the address of an error description for the
 *				  error being signaled.
 *
 *  Returns:
 *	NOTHING
 *
 *****/
PrivateFnDef int TheTLLDefaultErrorHandler (ERR_ErrorDescription *errorDescription) {
    ERR_ReportErrorOnStderr ("Trapped By Read-Eval-Print", errorDescription);
    UNWIND_ThrowException ();

    return 0;
}


/******************************
 *  Control Message Receiver  *
 ******************************/

/*---------------------------------------------------------------------------
 *****  The default read-eval-print loop control message receiver.
 *
 *  Arguments:
 *	errorDescription	- the address of the error description
 *				  associated with the 'receive' control
 *				  messages signal.
 *
 *  Returns:
 *	NOTHING
 *
 *  Notes:
 *	Currently, this routine is used to toggle the state of debugger
 *	accessibility.  It should be generalized to work with a message
 *	queue and/or shared memory to allow arbitrary messages to be sent
 *	to the interpreter.
 *
 *****/
PrivateFnDef int ReceiveControlMessages (
    ERR_ErrorDescription *Unused(errorDescription)
) {
/*****  Toggle the end-user-system switch  *****/
    Batchvision::ToggleDebuggerEnabled ();

    return 0;
}


/*********************************
 *****  Error Handler Setup  *****
 *********************************/

/*---------------------------------------------------------------------------
 * Note: The disconnect handler must be established before the default
 * handler to reduce the possibility of an infinite loop.
 *---------------------------------------------------------------------------
 */
PrivateFnDef void EstablishErrorHandlers () {
/*****  Establish...  *****/

/*****  ...a disconnect error handler...  *****/
    ERR_SetErrorHandler (
	EC__HangupSignal	, SIGNAL_HandleDisconnect
    );

/*****  ...a default error handler, ...  *****/
    ERR_SetErrorHandler (
	EC__AnError		, TheTLLDefaultErrorHandler
    );

/*****  ...a software terminate signal handler,  ...  *****/
    ERR_SetErrorHandler (
	EC__TerminateSignal	, SIGNAL_HandleDisconnect
    );
	 
/*****  ...an interrupt handler, ...  *****/
    ERR_SetErrorHandler (
	EC__InterruptSignal	, SIGNAL_HandleInterrupt
    );

/*****  ...an optional quit, ...  *****/
    if (GOPT_GetSwitchOption ("AllowQuit") > 0) ERR_SetErrorHandler (
	EC__QuitSignal		, SIGNAL_HandleDisconnect
    );

/*****  ...and a control message receiver.  *****/
    ERR_SetErrorHandler (
	EC__UserSignal2		, ReceiveControlMessages
    );
}


/*********************
 *********************
 *****  Options  *****
 *********************
 *********************/

GOPT_BeginOptionDescriptions

/*****  Language Processor Options  *****/

/*----  Network Specification  ----*/
    GOPT_ValueOptionDescription (
	"NDFPathName"			, 'n', "NDFPathName"	, "/vision/network/NDF"
    )

    GOPT_ValueOptionDescription (
	"UserObjectSpaceIndex"		, 'U', "UserOSI"	, "0"
    )

    GOPT_ValueOptionDescription (
	"NetworkVersion"		, 'v', NilOf(char *), NilOf (char *)
    )

    GOPT_ValueOptionDescription (
	"StartupExpression"		, 'x', "VisionStartExpr", ""
    )

/*----  Network Update and Maintenance  ----*/
    GOPT_ValueOptionDescription	(
	"XUSpecPathName"		, 'I', NilOf(char *), NilOf (char *)
    )

    GOPT_ValueOptionDescription (
	"CompactNetwork"   		, 'C', NilOf(char *), NilOf (char *)
    )
    GOPT_SwitchOptionDescription (
	"NetCompactionStatisticsOnly"	, 's', NilOf (char *)
    )

    GOPT_ValueOptionDescription (
	"OSDPathName"			, 'o', "OSDPathName"	, NilOf (char *)
    )
    GOPT_ValueOptionDescription (
	"RebuildNDF"	    		, 'R', NilOf(char *), NilOf (char *)
    )
    GOPT_SwitchOptionDescription (
	"BootStrap"	    		, 'b', "VisionBoot"
    )

/*----  Privileges  ----*/
    GOPT_SwitchOptionDescription (
	"Administrator"			, 'a', "VisionAdm"
    )
    GOPT_SwitchOptionDescription (
	"Developer"			, 'd', "VisionDev"
    )

/*----  Interrupt and Front-end Support  ----*/
    GOPT_SwitchOptionDescription (
	"IgnoreInterrupts"		, 'i', NilOf (char *)
    )
    GOPT_SwitchOptionDescription (
	"AllowQuit"			, 'q', NilOf (char *)
    )
    GOPT_SwitchOptionDescription (
	"FromEditor"			, 'E', NilOf (char *)
    )

/*----  Tuning Parameters  ----*/
    GOPT_ValueOptionDescription (
	"LargeTaskSize"			, 't', "LargeTaskSize"	, "0"
    )

/*****  Front End Options  *****/
    GOPT_ValueOptionDescription (
	"WhichRS"			, 'F', "WhichRS"	, "/vision/bin/batchvision"
    )
    GOPT_SwitchOptionDescription (
	"Debug"				, 'D', NilOf (char *)
    )
    GOPT_ValueOptionDescription (
	"ProfileFile"			, 'S', "VisionProfile", NilOf (char *)
    )

/*****  Combined Options  *****/

GOPT_EndOptionDescriptions;


/******************************
 ******************************
 *****  The Main Program  *****
 ******************************
 ******************************/

int __cdecl main (int sArgv, char *pArgv[]) {
    Batchvision::Main iMain (sArgv, pArgv);
    if (!iMain.runnable ())
	return iMain.exitStatus ();

/*****  Make a permanent Nil IObject, ... (added by m2)  *****/
    IOBJ_TheNilIObject = IOBJ_PackIntIObject (IOBJ_IType_Nil, 0);

/*****  ... set up the signal handlers, ...  *****/
    if (GOPT_GetSwitchOption ("IgnoreInterrupts") > 0)
        SIGNAL_SetUpSignalIgnorance ();

    SIGNAL_SetUpSignalHandlers ();

/*****  ... initialize other system facilities, ...  *****/
    InitializeSystemFacilities ();

    UNWIND_Try {
    //  ... establish error handlers, ...
	EstablishErrorHandlers ();

    //  ... and do something useful:
	iMain.processEvents ();
    } UNWIND_Finally {
	ShutDownSystemFacilities ();
    } UNWIND_EndTryFinally;

/*****  ...and exit successfully, it is to be hoped (see MS/Word grammar)!  *****/
    return iMain.getExitValue ();
}


/*********************************
 *********************************
 *****  Facility Definition  *****
 *********************************
 *********************************/

FAC_DefineFacility {
    switch (FAC_RequestTypeField) {
    FAC_FDFCase_FacilityIdAsString (Batchvision);
    FAC_FDFCase_FacilityDescription ("The Main Program");
    default:
        break;
    }
}


/************************** SOURCE FILE HISTORY: ************************/
/************************************************************************
  main.c 1 replace /users/mjc/system
  860226 16:23:34 mjc create

 ************************************************************************/
/************************************************************************
  main.c 2 replace /users/mjc/system
  860323 20:23:25 mjc Added signal catching code for all catchable signals

 ************************************************************************/
/************************************************************************
  main.c 3 replace /users/mjc/system
  860326 08:00:55 mjc Added facility definition

 ************************************************************************/
/************************************************************************
  main.c 4 replace /users/jad
  860403 12:59:14 mjc Converted signal mask to ~(unsigned long)0 - NO change to stderr

 ************************************************************************/
/************************************************************************
  main.c 5 replace /users/mjc/system
  860404 09:15:11 mjc Add startup initialization calls to 'selector' and 'vmachine'

 ************************************************************************/
/************************************************************************
  main.c 6 replace /users/jad
  860417 15:57:51 jad added hooks to make the rs work from the editor

 ************************************************************************/
/************************************************************************
  main.c 7 replace /users/jad/system
  860422 15:16:16 jad use new io module

 ************************************************************************/
/************************************************************************
  main.c 8 replace /users/hjb/system
  860428 19:09:13 hjb added facility to initialize LTYPE

 ************************************************************************/
/************************************************************************
  main.c 9 replace /users/jck/system
  860429 06:31:04 jck MAGIC and RSLANG added to initialization list

 ************************************************************************/
/************************************************************************
  main.c 10 replace /users/mjc/makework
  860503 16:36:43 mjc Added 'UserSystem' option, interrupted system call
signal restart and signal specific error codes for SIGHUP and the alarms

 ************************************************************************/
/************************************************************************
  main.c 11 replace /users/jad/system
  860507 17:25:56 jad added an error message to the USER1 interrupt

 ************************************************************************/
/************************************************************************
  main.c 12 replace /users/jad/system
  860507 19:59:22 jad temporary fix, need to make a main.i file which
contains the gopt options to be used by the RS and the editor

 ************************************************************************/
/************************************************************************
  main.c 13 replace /users/jad/system
  860508 13:08:18 jad created main.i file for GOPT
descriptions

 ************************************************************************/
/************************************************************************
  main.c 14 replace /users/mjc/system
  860515 13:30:30 mjc Release definition of 'RTvector' initialization

 ************************************************************************/
/************************************************************************
  main.c 15 replace /users/mjc/system
  860526 20:33:46 mjc Reversed order of V and rtVECTOR initialization

 ************************************************************************/
/************************************************************************
  main.c 16 replace /users/mjc/system
  860531 10:44:05 mjc Added 'envir' as an initialized facility

 ************************************************************************/
/************************************************************************
  main.c 17 replace /users/mjc/system
  860604 19:25:55 mjc Release of primitive function handling facility

 ************************************************************************/
/************************************************************************
  main.c 18 replace /users/mjc/system
  860606 13:26:24 mjc Eliminated 'rtVECTOR' from startup sequence

 ************************************************************************/
/************************************************************************
  main.c 19 replace /users/mjc/system
  860615 18:53:58 mjc Delete 'ltype' declarations

 ************************************************************************/
/************************************************************************
  main.c 20 replace /users/jad/system
  860914 11:15:21 mjc Release split version of 'M' and 'SAVER'

 ************************************************************************/
/************************************************************************
  main.c 21 replace /users/mjc/system
  861002 18:34:12 mjc Release of changes in support of new list architecture

 ************************************************************************/
/************************************************************************
  main.c 22 replace /users/jck/system
  861015 09:02:46 jck Added optional ignorance of interrupt,quit and hangup signals

 ************************************************************************/
/************************************************************************
  main.c 23 replace /users/jad/system
  861112 10:50:13 jad turn off -lmalloc small blocks because of a bug in them

 ************************************************************************/
/************************************************************************
  main.c 24 replace /users/jad/system
  861116 15:57:10 jad changed the order of the primfns initialization to after vmachine

 ************************************************************************/
/************************************************************************
  main.c 25 replace /users/mjc/system
  870215 23:04:23 lcn Changed set of initialized modules

 ************************************************************************/
/************************************************************************
  main.c 26 replace /users/jad/system
  870317 12:29:07 jad modify interrupt handling

 ************************************************************************/
/************************************************************************
  main.c 27 replace /users/jad/system
  870324 13:11:25 jad modified interrupt handling to only be on during execution

 ************************************************************************/
/************************************************************************
  main.c 28 replace /users/mjc/translation
  870524 09:40:02 mjc Shortened all identifiers to <= 31 characters

 ************************************************************************/
/************************************************************************
  main.c 29 replace /users/mjc/system
  870525 13:50:38 mjc Changed 'FacilityName' to correct global id case insensitivity problem on VAX

 ************************************************************************/
/************************************************************************
  main.c 30 replace /users/mjc/system
  870614 20:04:38 mjc Convert UNIX specific include files to 'stdoss.h'

 ************************************************************************/
/************************************************************************
  main.c 31 replace /users/m2/backend
  890503 15:25:03 m2 Return STD_ exit values, Set up VAX signals

 ************************************************************************/
/************************************************************************
  main.c 32 replace /users/m2/backend
  890927 15:32:13 m2 Made IOBJ_TheNilIObject a global var instead of a procedure call

 ************************************************************************/
