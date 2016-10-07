/*****  Vca_VMainProgram Implementation  *****/

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

#include "Vca_VMainProgram.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"

#include "Vca_IDirectory.h"

#include "Vca_VApplication.h"
#include "Vca_VApplicationContext.h"

#include "V_VTwiddler.h"

#include "VTransientServices.h"


#ifdef _WIN32

/*****************************************************
 *****************************************************
 *****                                           *****
 *****  Vca::VMainProgram::WindowsServiceStatus  *****
 *****                                           *****
 *****************************************************
 *****************************************************/

namespace Vca {
    class VMainProgram::WindowsServiceStatus : public SERVICE_STATUS {
	DECLARE_FAMILY_MEMBERS (WindowsServiceStatus, SERVICE_STATUS);

    //  Aliases
    public:
	typedef Application::RunState RunState;

    //  Construction
    public:
	WindowsServiceStatus (RunState xRunState);

    //  Destruction
    public:
	~WindowsServiceStatus () {
	}

    //  Update
    public:
	void setStateTo (RunState xRunState);
    };
}

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VMainProgram::WindowsServiceStatus::WindowsServiceStatus (RunState xRunState) {
    dwServiceType		= SERVICE_WIN32;
    dwCurrentState		= SERVICE_STOPPED;
    dwControlsAccepted		= SERVICE_ACCEPT_STOP + SERVICE_ACCEPT_PAUSE_CONTINUE;
    dwWin32ExitCode		= 0;
    dwServiceSpecificExitCode	= 0;
    dwCheckPoint		= 0;
    dwWaitHint			= 0;

    setStateTo (xRunState);
}

void Vca::VMainProgram::WindowsServiceStatus::setStateTo (RunState xRunState) {
    switch (xRunState) {
    case Application::RunState_AwaitingStart:
    case Application::RunState_Starting:
	dwCurrentState = SERVICE_START_PENDING;
	break;
    case Application::RunState_Running:
	dwCurrentState = SERVICE_RUNNING;
	break;
    case Application::RunState_Pausing:
	dwCurrentState = SERVICE_PAUSE_PENDING;
	break;
    case Application::RunState_Paused:
	dwCurrentState = SERVICE_PAUSED;
	break;
    case Application::RunState_Resuming:
	dwCurrentState = SERVICE_CONTINUE_PENDING;
	break;
    case Application::RunState_Stopping:
    case Application::RunState_AwaitingStop:
	dwCurrentState = SERVICE_STOP_PENDING;
	break;
    case Application::RunState_Stopped:
	dwCurrentState = SERVICE_STOPPED;
	break;
    }
}
#endif


/*******************************************************
 *******************************************************
 *****                                             *****
 *****  Vca::VMainProgram::Application::RunRecord  *****
 *****                                             *****
 *******************************************************
 *******************************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VMainProgram::Application::RunRecord::RunRecord ()
    : m_pIRunStateReceiver (this), m_xRunState (RunState_AwaitingStart)
#ifdef _WIN32
    , m_bService (false)
#endif
{
}

/***********************
 ***********************
 *****  Callbacks  *****
 ***********************
 ***********************/

void Vca::VMainProgram::Application::RunRecord::OnData (IRunStateReceiver *pRole, U08 xRunState) {
    setRunStateTo (static_cast<RunState>(xRunState));
}

void Vca::VMainProgram::Application::RunRecord::OnEnd_() {
}

void Vca::VMainProgram::Application::RunRecord::OnError_(IError *pInterface, VString const &rMessage) {
}


/**************************
 **************************
 *****  Registration  *****
 **************************
 **************************/

void Vca::VMainProgram::Application::RunRecord::registerApplication (VApplication *pApplication) {
    setRunStateTo (pApplication->runState ());

    IRunStateReceiver::Reference pRunStateReceiver;
    getRole (pRunStateReceiver);
    pApplication->onRunStateChangeNotify (pRunStateReceiver);
}

void Vca::VMainProgram::Application::RunRecord::registerApplication (
    char const *pName, Application *pApplication
) {
#ifdef _WIN32
    m_hService = RegisterServiceCtrlHandlerEx (
	pName, Application::ServiceController, pApplication
    );  // ... must return true after RegisterServiceCtrlHandlerEx is called
    m_bService = true;
#endif
}

/********************
 ********************
 *****  Update  *****
 ********************
 ********************/

void Vca::VMainProgram::Application::RunRecord::setRunStateTo (RunState xRunState) {
    if (m_xRunState != xRunState) {
	m_xRunState = xRunState;
	pushState ();
    }
}

void Vca::VMainProgram::Application::RunRecord::pushState () const {
#ifdef _WIN32
	if (m_bService) {
	    WindowsServiceStatus iServiceStatus (m_xRunState);
	    SetServiceStatus (m_hService, &iServiceStatus);
	}
#endif
}


/********************************************
 ********************************************
 *****                                  *****
 *****  Vca::VMainProgram::Application  *****
 *****                                  *****
 ********************************************
 ********************************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VMainProgram::Application::Application (
    VMainProgram *pMain, VString const &rName
) : m_pDefaultContext (pMain->appContext ()), m_iName (rName), m_pRunRecord (new RunRecord ()) {
    pMain->install (this);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VMainProgram::Application::~Application () {
}


/*********************
 *********************
 *****  Control  *****
 *********************
 *********************/

void Vca::VMainProgram::Application::registerApplication (VApplication *pApplication) {
    if (pApplication) {
	pApplication->getRole (m_pIStop);
	pApplication->getRole (m_pIPauseResume);

	m_pRunRecord->registerApplication (pApplication);
    }
}

void Vca::VMainProgram::Application::stop_(bool bHardStop) {
    if (m_pIStop)
	m_pIStop->Stop (bHardStop);
}

void Vca::VMainProgram::Application::pause_() {
    if (m_pIPauseResume)
	m_pIPauseResume->Pause ();
}

void Vca::VMainProgram::Application::resume_() {
    if (m_pIPauseResume)
	m_pIPauseResume->Resume ();
}


/*************************************
 *************************************
 *****  Windows Service Support  *****
 *************************************
 *************************************/

#ifdef _WIN32
DWORD WINAPI Vca::VMainProgram::Application::ServiceController (
    DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext
) {
    ThisClass::Pointer pApplication (reinterpret_cast<ThisClass*>(lpContext));
    return pApplication
	? pApplication->serviceController (dwControl, dwEventType, lpEventData)
	: ERROR_CALL_NOT_IMPLEMENTED;
}

DWORD Vca::VMainProgram::Application::serviceController (
    DWORD dwControl, DWORD dwEventType, LPVOID lpEventData
) {
    DWORD xResultCode = NO_ERROR;

    switch (dwControl) {
    case SERVICE_CONTROL_CONTINUE:
	resume_();	
	break;
    case SERVICE_CONTROL_INTERROGATE:
	break;
    case SERVICE_CONTROL_PAUSE:
	pause_();
	break;
    case SERVICE_CONTROL_STOP:
	stop_();
	break;
    default:
	xResultCode = ERROR_CALL_NOT_IMPLEMENTED;
	break;
    }
    m_pRunRecord->pushState ();
    return xResultCode;
}

bool Vca::VMainProgram::Application::serviceMain (DWORD argc, LPTSTR *argv) {
    m_pRunRecord->registerApplication (argv[0], this);

    VCohort::Claim iThisMutex (this);
    m_pCurrentContext.setTo (new VApplicationContext (argc, argv, m_pDefaultContext));
    start_();

    return true;
}
#endif


/*******************************
 *******************************
 *****                     *****
 *****  Vca::VMainProgram  *****
 *****                     *****
 *******************************
 *******************************/

/*********************
 *********************
 *****  Globals  *****
 *********************
 *********************/

Vca::VMainProgram::Pointer Vca::VMainProgram::g_pTheMainMain;

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

Vca::VMainProgram::VMainProgram (int argc, argv_t argv)
    : m_iVcaClaim (VCohort::Vca ())
    , m_pAppContext (new VApplicationContext (argc, argv))
    , m_bInitialized (Start (&m_iInitializationError))
{
//  If the root directory doesn't exist, GetRootDirectory creates it.  Before
//  doing so, it claims the Vca cohort to ensure that all directory objects
//  live there.  That may not be necessary, but that's the way it's done now.
//  Since this thread may forever lock and hold the Vca cohort, make sure
//  the directory can be created by creating it now...
    IDirectory::Reference pRootDirectory;
    GetRootDirectory (pRootDirectory);

    if (g_pTheMainMain.setIfNil (this)) {
	if (m_pAppContext->commandSwitchValue ("-slackerSnitch"))
	    VTypeInfoHolderInstance::SlackerTracker::ShowSlackers ();
    }
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

Vca::VMainProgram::~VMainProgram () {
    if (g_pTheMainMain.clearIf (this) && m_bInitialized) {
	Shutdown ();
	Stop ();
    }
}


/*************************************
 *************************************
 *****  Windows Service Support  *****
 *************************************
 *************************************/

#ifdef _WIN32
void WINAPI Vca::VMainProgram::ServiceMain (DWORD argc, LPTSTR *argv) {
    if (g_pTheMainMain.isNil () || !g_pTheMainMain->serviceMain (argc, argv)) {
	SERVICE_STATUS_HANDLE hService = RegisterServiceCtrlHandlerEx (
	    argv[0], Application::ServiceController, 0
	);
	WindowsServiceStatus iServiceStatus (Application::RunState_Stopped);
	SetServiceStatus (hService, &iServiceStatus);
    }
}

bool Vca::VMainProgram::serviceMain (DWORD argc, LPTSTR *argv) {
    Application::Reference pApplication;
    switch (m_aApplications.elementCount ()) {
    case 0:
	break;
    case 1:
	pApplication.setTo (m_aApplications[0]);
	break;
    default:
	for (unsigned int xApp = 0; pApplication.isNil () && xApp < applicationCount (); xApp++) {
	    Application::Reference pApp (m_aApplications[xApp]);
	    if (strcmp (pApp->appName (), argv[0]) == 0) {
		pApplication.claim (pApp);
	    }
	}
	break;
    }
    return pApplication && pApplication->serviceMain (argc, argv);
}
#endif


/************************************
 ************************************
 *****  Application Management  *****
 ************************************
 ************************************/

void Vca::VMainProgram::install (Application *pApplication) {
    m_aApplications.Append (1);
    m_aApplications[m_aApplications.elementCount () - 1].setTo (pApplication);
}

bool Vca::VMainProgram::runnable () {
    if (!initialized ())
	return false;

//  Secondary initialization guard:
    static V::VTwiddler s_bCalledAlready;
    if (!s_bCalledAlready.setIfClear ())
	return true;

//  Windows services are created here:
#ifdef _WIN32
    unsigned int const cApps = applicationCount ();
    if (cApps > 0) {
	VkDynamicArrayOf<SERVICE_TABLE_ENTRY> Services (cApps + 1);
	unsigned int xApp = 0;
	while (xApp < cApps) {
	    SERVICE_TABLE_ENTRY &rService = Services[xApp];
	    rService.lpServiceName = const_cast<char*>(m_aApplications[xApp]->appName ().content ());
	    rService.lpServiceProc = ServiceMain;
	    xApp++;
	} {
	    SERVICE_TABLE_ENTRY &rService = Services[xApp];
	    rService.lpServiceName = 0;
	    rService.lpServiceProc = 0;
	}

    /****************
    *  When this program is run as a windows service, StartServiceCtrlDispatcher
    *  starts a new thread to run the service and blocks until that the Windows
    *  SCM tells it to exit.  The Vca claim held by this thread must be released
    *  and reacquired if appropriate or this process will hang...
    ****************/
	m_iVcaClaim.release ();
	if (StartServiceCtrlDispatcher (Services.elementArray ()))
	    return false;	 // ... we just ran as a windows service
	DWORD const xLastError = GetLastError ();
	if (xLastError != ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
	    return false;	//  ... we should have run as a windows service but failed

	m_iVcaClaim.acquire ();	//  ... we're not a windows service
    }
#else
//  Unix daemons are created here...
    transientServicesProvider ()->updateBackgroundSwitch (
	m_pAppContext->commandSwitchValue ("detach") ||
	m_pAppContext->commandSwitchValue ("daemon")
    );
#endif

    return true;
}


/******************************
 ******************************
 *****  Startup/Shutdown  *****
 ******************************
 ******************************/

bool Vca::VMainProgram::onStartup () {
//  Assume we're good to go until proven otherwise...
    bool bGoodToGo = true;

//  ... and ask each registered application to start:
    unsigned int xApplication = 0;
    while (bGoodToGo && xApplication < m_aApplications.elementCount ()) {
	bGoodToGo = m_aApplications[xApplication++]->start_();
    }

//  If everything started OK, return successfully ...
    if (bGoodToGo)
	return true;

//  ... if not, back out the initialized applications...
    while (xApplication > 0) {
	m_aApplications[--xApplication]->stop_();
    }

//  ... and return failure:
    return false;
}

void Vca::VMainProgram::onShutdown () {
    unsigned int xApplication = m_aApplications.elementCount ();
    while (xApplication > 0) {
	m_aApplications[--xApplication]->stop_();
    }
}


/******************************
 ******************************
 *****  Event Processing  *****
 ******************************
 ******************************/

void Vca::VMainProgram::eventLoopImplementation_() {
    VCohortManager iCM;
    if (iCM.attachCohortEvents ())
	iCM.doEvents ();
}

int Vca::VMainProgram::processEvents () {
    if (runnable () && onStartup ()) {
	eventLoopImplementation_();
	onShutdown ();
    }
    return exitStatus ();
}
