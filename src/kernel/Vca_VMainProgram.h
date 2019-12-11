#ifndef Vca_VMainProgram_Interface
#define Vca_VMainProgram_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VProgramContext.h"
#include "Vca_VRolePlayer.h"

#include "Vca_VApplicationContext.h"

#include "Vca_VCohort.h"
#include "V_VString.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_IStop.h"
#include "Vca_IPauseResume.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VApplication;

    /**
     * Main program class providing convenience functions for subclasses intended to behave as main program controllers.
     */
    class Vca_Main_API VMainProgram : public VProgramContext {
	DECLARE_FAMILY_MEMBERS (VMainProgram, VProgramContext);

    //  Aliases
    public:
	typedef VApplicationContext::Reference AppContextReference;
	typedef VMainProgram::Pointer MainPointer;

    //  ApplicationStatus
    public:
	class ApplicationStatus;

    //  Application
    public:
	class Vca_Main_API Application : public VRolePlayer, public VApplicationContext::ApplicationAddin {
	    DECLARE_ABSTRACT_RTTLITE (Application, VRolePlayer);

	    friend class VMainProgram;

	//  RunRecord
	public:
	    class RunRecord : public VRolePlayer {
		DECLARE_CONCRETE_RTTLITE (RunRecord, VRolePlayer);

		friend class Application;

	    //  Construction
	    public:
		RunRecord ();

	    //  Destruction
	    private:
		~RunRecord () {
		}

	    //  Base Class Roles
	    public:
		using BaseClass::getRole;

	    //  IRunStateReceiver Role
	    private:
		VRole<ThisClass,IRunStateReceiver> m_pIRunStateReceiver;
	    public:
		void getRole (IRunStateReceiver::Reference &rpRole) {
		    m_pIRunStateReceiver.getRole (rpRole);
		}

	    //  IRunStateReceiver Methods
	    public:
		void OnData (IRunStateReceiver *pRole, U08 xRunState);

	    //  IClient Callbacks
	    private:
		virtual void OnEnd_() OVERRIDE;
		virtual void OnError_(IError *pInterface, VString const &rMessage) OVERRIDE;

	    //  Application Registration
	    private:
		void registerApplication (char const *pName, Application *pApplication);
		void registerApplication (VApplication *pApplication);

	    //  Access
	    public:
		RunState runState () const {
		    return m_xRunState;
		}

	    //  Update
	    private:
		void setRunStateTo (RunState xRunState);
		void pushState () const;

	    //  State
	    private:
		RunState		m_xRunState;
	#ifdef _WIN32
		SERVICE_STATUS_HANDLE	m_hService;
		bool			m_bService;
	#endif
	    };

	//  Construction
	protected:
	    Application (VMainProgram *pMain, VString const &rName);

	//  Destruction
	protected:
	    ~Application ();

	//  Roles
	public:
	    using BaseClass::getRole;

	//  Access
	public:
	    VApplicationContext *appContext () const {
		return m_pCurrentContext ? m_pCurrentContext : m_pDefaultContext;
	    }
	    VString const &appName () const {
		return m_iName;
	    }

	//  Query
	public:
	    virtual RunState runState () const {
		return m_pRunRecord->runState ();
	    }

	//  Control
	protected:
	    void registerApplication (VApplication *pApplication);

	    virtual bool start_() = 0;
	    virtual void stop_(bool bHardStop = false);
	    virtual void pause_();
	    virtual void resume_();

	//  Callbacks
	private:
	    void onStart ();
	    void onStop  ();
	    void onPause ();
	    void onResume();

	//  Windows Service Support
	private:
#ifdef _WIN32
	    static DWORD WINAPI ServiceController (
		DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext
	    );
	    DWORD serviceController (
		DWORD dwControl, DWORD dwEventType, LPVOID lpEventData
	    );
	    bool serviceMain (DWORD argc, LPTSTR *argv);
#endif

	//  State
	private:
	    AppContextReference		const	m_pDefaultContext;
	    VString			const	m_iName;
	    RunRecord::Reference	const	m_pRunRecord;
	    AppContextReference			m_pCurrentContext;
	    IStop::Reference			m_pIStop;
	    IPauseResume::Reference		m_pIPauseResume;
	};
	friend class Application;

    //  WindowsServiceStatus
    public:
	class WindowsServiceStatus;

    //  Construction
    public:
	VMainProgram (int argc, argv_t argv);

    //  Destruction
    public:
	~VMainProgram ();

    //  Access
    public:
	static ThisClass *TheMainMain () {
	    return g_pTheMainMain;
	}
	VApplicationContext *appContext () const {
	    return m_pAppContext;
	}
	unsigned int applicationCount () const {
	    return m_aApplications.elementCount ();
	}

	operator VApplicationContext* () const {
	    return m_pAppContext;
	}

    //  Application Management
    private:
	void install (Application *pApplication);

    //  Windows Service Support
    private:
#ifdef _WIN32
	static void WINAPI ServiceMain (DWORD argc, LPTSTR *argv);
	bool serviceMain (DWORD argc, LPTSTR *argv);
#endif

    //  Cohort Management
    protected:
	void releaseCohort () {
	    m_iVcaClaim.release ();
	}

    //  Event Processing
    private:
	bool initialized () const {
	    return m_bInitialized && m_pAppContext.isntNil ();
	}
    public:
	bool runnable ();

    private:
	virtual bool onStartup ();
	virtual void onShutdown ();
    protected:
	virtual void eventLoopImplementation_();
    public:
	int processEvents ();

    //  Status
    public:
	int exitStatus () const {
	    return initialized () ? m_pAppContext->getExitStatus () : ErrorExitValue;
	}
	VString const &initializationError () const {
	    return m_iInitializationError;
	}

    //  State
    private:
	static Pointer				g_pTheMainMain;

	VCohort::Claim				m_iVcaClaim;
	AppContextReference const		m_pAppContext;
	VkDynamicArrayOf<Application::Reference>m_aApplications;
	VString					m_iInitializationError;
	bool					m_bInitialized;
    };
}



#endif
