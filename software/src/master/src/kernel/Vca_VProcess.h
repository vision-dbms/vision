#ifndef Vca_VProcess_Interface
#define Vca_VProcess_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "Vca_IProcess.h"

#include "V_VCOS.h"
#include "VkDynamicArrayOf.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

/**********************
 *----  VProcess  ----*
 **********************/

namespace Vca {
    class Vca_API VProcess : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VProcess, VRolePlayer);

    //  Command
    public:
	class Command : public VTransient {
	    DECLARE_FAMILY_MEMBERS (Command, VTransient);

	//  Construction
	public:
	    Command (char const *pCommand) {
		setTo (pCommand);
	    }
	    Command () {
	    }

	//  Destruction
	public:
	    ~Command () {
	    }

	//  Access
	public:
	    char **argv () {
		return m_iArgVector.elementArray ();
	    }

	//  Update
	public:
	    char **setTo (char const *pCommand);

	//  State
	private:
	    V::VCOS			m_iArgStorage;
	    VkDynamicArrayOf<char*>	m_iArgVector;
	};

    //  Info
    public:
#if defined(_WIN32)
	typedef PROCESS_INFORMATION Info;
#else
	typedef pid_t Info;
#endif

    //  Construction
    public:
	VProcess (Info const &rProcessData);

    //  Destruction
    private:
	~VProcess ();

    //  Base Roles
    public:
	using BaseClass::getRole;

    //  IProcess Role
    private:
	VRole<ThisClass,IProcess> m_pIProcess;
    public:
	void getRole (VReference<IProcess>&rpRole) {
	    m_pIProcess.getRole (rpRole);
	}

    //  IProcess Methods
    public/*private*/:
	void WaitForTermination (IProcess *pRole, IVReceiver<S32> *pResultReceiver);

    //  Use
    private:
	void onClosedStdio ();
    public:
	void onClosedStdin ();
	void onClosedStdout ();
	void onClosedStderr ();

	void setTerminateOnClosedStdio (bool bValue = true) {
	    m_bTerminatingOnClosedStdio = bValue;
	}

	void waitForTermination (int *pResultReturn);

    //  State
    protected:
	Info const m_iProcessInfo;
	bool m_bTerminatingOnClosedStdio;
	bool m_bNoStdin;
	bool m_bNoStdout;
	bool m_bNoStderr;
    };
}


#endif
