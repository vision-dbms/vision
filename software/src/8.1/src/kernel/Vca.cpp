/**
 * @file
 * Vca namespace implementation; provides definition of several global Vca functions.
 */

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

#include "Vca.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


#include "Vca_VcaSelf.h"
#include "Vca_VcaListener.h"

#include "Vca_VStatus.h"
#include "Vca_VDirectory.h"
#include "Vca_VcaDirectoryBuilder.h"
#include "Vca_VcaCredentialsProvider.h"

#include "Vca_VInterfaceQuery.h"

#include "V_VTime.h"


/******************
 ******************
 *****  Self  *****
 ******************
 ******************/

namespace {
    Vca::VcaSelf::Reference g_pSelf;
}

Vca::VcaSelf *Vca::self () {
    if (g_pSelf.isNil ())
	g_pSelf.setTo (new VcaSelf ());
    return g_pSelf;
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

/*********************
 *****  Globals  *****
 *********************/

#if defined(__VMS)

#elif defined(_WIN32)
static WSADATA	g_iWinsockInitializationData;
static int	g_xWinsockInitializationError = WSANOTINITIALISED;

static bool GetInitializationStatus (VString *pMessageReturn) {
    if (!g_xWinsockInitializationError || !pMessageReturn)
	return !g_xWinsockInitializationError;

    VkStatus iStatus;
    iStatus.MakeErrorStatus (g_xWinsockInitializationError);
    iStatus.getDescription (*pMessageReturn);

    return false;
}

#else
static bool		g_bSignalHandlingInitialized = false;
static struct sigvec	g_iInitialHandlerForSIGPIPE;
static struct sigvec	g_iInitialHandlerForSIGCLD;

#endif

namespace {
    static Vca::VDirectory::Reference pTheRootDirectory;
    static Vca::VDirectoryBuilder::Reference pTheBuilder;
    static Vca::VcaCredentialsProvider::Reference pTheCredentialsProvider;
}

/*********************
 *****  Startup  *****
 *********************/

bool Vca::Start (VString *pMessageReturn) {
#if defined(__VMS)

/*****************
 *----  VMS  ----*
 *****************/

    bool bGoodToGo = true;

#elif defined(_WIN32)

/*******************
 *----  Win32  ----*
 *******************/

    bool bGoodToGo = GetInitializationStatus (pMessageReturn);

#else

/******************
 *----  Unix  ----*
 ******************/

/*****  Ignore SIGPIPE, Child Status Change and Window Change  *****/
    if (!g_bSignalHandlingInitialized) {
	struct sigvec vec;
	vec.sv_handler	= (STD_SignalHandlerType)SIG_IGN;
	vec.sv_onstack	= 0;
	vec.sv_mask	= ~(STD_maskType)0;

	STD_sigvector (SIGPIPE, &vec, &g_iInitialHandlerForSIGPIPE);

/*----------------------------------------------------------------*
 ***  Death of child signals must be set to SIG_DFL if 'wait'	***
 ***  is to operate correctly.					***
 *----------------------------------------------------------------*/
	vec.sv_handler = (STD_SignalHandlerType)SIG_DFL;
	STD_sigvector (SIGCLD, &vec, &g_iInitialHandlerForSIGCLD);

	g_bSignalHandlingInitialized = true;
    }

    bool bGoodToGo = true;

#endif

    return bGoodToGo;
}


/**********************
 *****  Shutdown  *****
 **********************/

bool Vca::Stop (VString *pMessageReturn) {

#if defined(__VMS)

/*********************
 *----  OpenVMS  ----*
 *********************/

    return true;

#elif defined(_WIN32)

/*******************
 *----  Win32  ----*
 *******************/

    return GetInitializationStatus (pMessageReturn);

#else

/******************
 *----  Unix  ----*
 ******************/

/*****  Restore SIGPIPE, Child Status Change and Window Change  *****/
    if (g_bSignalHandlingInitialized) {
	STD_sigvector (SIGPIPE, &g_iInitialHandlerForSIGPIPE, NilOf (struct sigvec*));
	STD_sigvector (SIGCLD , &g_iInitialHandlerForSIGCLD , NilOf (struct sigvec*));

	g_bSignalHandlingInitialized = false;
    }
    return true;

#endif
}


/******************************************************************************
 * Routine: Shutdown
 *	Closes all the listeners available in this site. 
 * Closes all connections that exist from this site (peer) to all other peers.
******************************************************************************/

namespace {
    bool g_bAtExit = false;
    bool s_bStillAlive = true;
    V::VTime s_iAbsoluteStopTime;
}

void Vca::setAtExit () {
    g_bAtExit = true;
}

bool Vca::isAtExit () {
    return g_bAtExit;
}

void Vca::Shutdown () {
    if (s_bStillAlive) {
	s_bStillAlive = false;			//  ... set the switch that eventually disables all event loops.

	s_iAbsoluteStopTime.setToNow ();	//  ... set the absolute time at which all event loops must terminate immediately.
	s_iAbsoluteStopTime += 1000000 * static_cast<U64>(
	    V::GetEnvironmentValue ("VcaMaximumShutdownWaitSeconds", static_cast<double>(300.0)) // ... default == 5 minutes
	);

	if (g_pSelf) {				// ... and tell myself to stop, ...
	    IPeer::Reference pSelfRole;		// ... arranging to process the stop in the thread currently processing Vca events.
	    g_pSelf->getRole (pSelfRole);
	    pSelfRole->OnDone ();
	} else {
	    VcaListener::CloseListeners ();
	}
    }
}

bool Vca::ShutdownCalled () {
    return !s_bStillAlive;
}

bool Vca::EventsDisabled (size_t &rsTimeoutMSEC, V::VTime const &rLastEventTime) {
//  If shutdown hasn't been called yet, do nothing, ...
    if (s_bStillAlive)
	return false;

//  Otherwise, compute the lesser of our relative and absolute stop times, ...
    V::VTime iStopTime (rLastEventTime);
    iStopTime += 1000000 * static_cast<U64>(
	V::GetEnvironmentValue ("VcaRelativeShutdownWaitSeconds", static_cast<double>(60.0)) // default == 60 seconds
    );

    if (iStopTime > s_iAbsoluteStopTime)
	iStopTime = s_iAbsoluteStopTime;

//  ... stopping immediately if our stop time has past, ...
    V::VTime const iNow;
    if (iStopTime <= iNow) {
	rsTimeoutMSEC = 0;
	return true;
    }

//  ... and waiting no longer than our stop time limit for more events if it hasn't.
    size_t const sMaxTimeout = static_cast<size_t>((iStopTime - iNow) / 1000);
    if (rsTimeoutMSEC > sMaxTimeout)
	rsTimeoutMSEC = sMaxTimeout;

    return false;
}


/******************************************************************************
 *  Routine: Exit
 *	Calls _exit () after ensuring that the AtExit handler in VCohort
 *  is never executed.
 ******************************************************************************/

static bool g_bDoingHardExit = false;

void Vca::Exit (int xExitStatus) {
    g_bDoingHardExit = true;
    _exit (xExitStatus);
}

bool Vca::ExitCalled () {
    return g_bDoingHardExit;
}


/**********************************
 **********************************
 *****  Credentials Provider  *****
 **********************************
 **********************************/

void Vca::GetCredentialsProvider (VcaCredentialsProvider::Reference &rpCredentialsProvider) {
    if (pTheCredentialsProvider.isNil ()) 
	pTheCredentialsProvider.setTo (new VcaCredentialsProvider ());

    rpCredentialsProvider.setTo (pTheCredentialsProvider);
}

void Vca::RegisterDirectoryBuilder (VDirectoryBuilder *pBuilder) {
    if (pBuilder)
	pTheBuilder.setTo (pBuilder);
}


/********************************
 ********************************
 *****  The Root Directory  *****
 ********************************
 ********************************/

void Vca::GetRootDirectory (IDirectory::Reference &rpRootDirectory) {
    if (pTheRootDirectory.isNil ()) {
	VCohort::Claim iClaim (VCohort::Vca (), false);

	if (pTheBuilder.isNil ()) 
	    pTheBuilder.setTo (new VcaDirectoryBuilder ());

	VDirectory::Reference pThisDirectory;
	pTheBuilder->create (pThisDirectory);
	pTheRootDirectory.interlockedSetIfNil (pThisDirectory);
    }
    pTheRootDirectory->getRole (rpRootDirectory);
}

/****************/

void Vca::GetObjectNames (
    IVReceiver<VkDynamicArrayOf<VString> const&> *pResultReceiver
) {
    IDirectory::Reference pRootDirectory;
    GetRootDirectory (pRootDirectory);
    pRootDirectory->GetEntries (pResultReceiver);
}

void Vca::GetObjectDescription (
    IVReceiver<VString const&> *pResultReceiver, VString const &rObjectName
) {
    IDirectory::Reference pRootDirectory;
    GetRootDirectory (pRootDirectory);
    pRootDirectory->GetDescription (pResultReceiver, rObjectName);
}

void Vca::GetObject (VInterfaceQuery const &rQuery, VString const &rName) {
    IDirectory::Reference pRootDirectory;
    GetRootDirectory (pRootDirectory);
    rQuery.getObject (rName, pRootDirectory);
}


/**********************
 **********************
 *****  Defaults  *****
 **********************
 **********************/

bool Vca::Default::TcpNoDelaySetting () {
//  Disable Nagle (set TCP_NODELAY) unless requested to do otherwise ...
    static bool const s_bTcpNoDelayEnabled = V::GetEnvironmentBoolean ("VcaTcpNoDelayEnabled", true);
    return s_bTcpNoDelayEnabled;
}


/*********************
 *********************
 *****  DllMain  *****
 *********************
 *********************/

#if defined(_WIN32)

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD xCallReason, LPVOID lpReserved) {
    switch (xCallReason) {
    case DLL_PROCESS_ATTACH:
	DisableThreadLibraryCalls (hModule);
	g_xWinsockInitializationError = WSAStartup (WINSOCK_VERSION, &g_iWinsockInitializationData);
	break;
    case DLL_THREAD_ATTACH:
	break;
    case DLL_THREAD_DETACH:
	break;
    case DLL_PROCESS_DETACH:
	if (!g_xWinsockInitializationError) {
	    g_xWinsockInitializationError = WSACleanup () ? WSAGetLastError () : WSANOTINITIALISED;
	}
	break;
    }
    return true;
}

#endif
