#ifndef Vca_VApplicationContext_Interface
#define Vca_VApplicationContext_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca_Main.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

#include "V_VCommandLine.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VApplication;

    class Vca_Main_API VApplicationContext : public VRolePlayer {
	DECLARE_CONCRETE_RTTLITE (VApplicationContext, VRolePlayer);

    //  Aliases
    public:
	typedef V::VCommandLine CommandLine;

    //  Friends
	friend class VApplication;

    //  Application Addin
    public:
	class Vca_Main_API ApplicationAddin {
	    DECLARE_NUCLEAR_FAMILY (ApplicationAddin);

	//  Run State
	public:
	    enum RunState {
		RunState_AwaitingStart,
		RunState_Starting,
		RunState_Running,
		RunState_Pausing,
		RunState_Paused,
		RunState_Resuming,
		RunState_Stopping,
		RunState_AwaitingStop,
		RunState_Stopped
	    };

	//  Run State Receiver
	public:
	    typedef IVReceiver<U08> IRunStateReceiver;
	};
	typedef ApplicationAddin::RunState RunState;

    //  Construction
    public:
	VApplicationContext (int argc, argv_t argv, ThisClass *pParent = 0);

    //  Destruction
    private:
	~VApplicationContext ();

    //  Command Line
    public:
	char const *arg0 () const {
	    return m_iCommandLine.arg0 ();
	}
	unsigned int argc () const {
	    return m_iCommandLine.argc ();
	}
	argv_t argv () const {
	    return m_iCommandLine.argv ();
	}

        /** Returns the command-line descriptor that the running application was started with. */
	CommandLine const &commandLine () const;

        /**
         * Retrieves a command-line parameter string value, falling back to environment variable as necessary.
         *
         * For example, the parameter @c xyz can be defined on command-line to equal @c StringValue by using the command-line syntax @c -xyz=StringValue.
         *
         * @param pTag the name of the parameter whose value should be retrieved.
         * @param pVariable the name of the environment variable whose value should be retrieved in the case that no command-line parameter was found matching @p pTag.
         * @param pDefaultValue the default value to return in the event that no suitable parameter value was found.
         * @return the value of the requested parameter.
         */
	char const *commandStringValue (
	    char const *pTag, char const *pVariable = 0, char const *pDefaultValue = 0
	) const;

        /**
         * Retrieves the state of a command-line switch.
         *
         * For example, the switch @c xyz can be turned on uzing the command-line syntax @c -xyz
         *
         * @param pTag the name of the switch whose value should be retrieved.
         * @param pVariable the name of the environment variable whose value should be retrieved in the case that no command-line parameter was found matching @p pTag.
         * @return true if the switch was set/enabled/on, false otherwise.
         */
	bool commandSwitchValue (
	    char const *pTag, char const *pVariable = 0
	) const;

        /**
         * Retrieves a command-line parameter numeric value, falling back to environment variable as necessary.
         *
         * For example, the parameter @c xyz can be defined on command-line to equal @c 5 by using the command-line syntax @c -xyz=5.
         *
         * @param[out] rValue the reference by which the value should be returned.
         * @param pTag the name of the parameter whose value should be retrieved.
         * @param pVariable the name of the environment variable whose value should be retrieved in the case that no command-line parameter was found matching @p pTag.
         * @return true on success, false otherwise.
         */
	bool getCommandValue (
	    size_t &rValue, char const *pTag, char const *pVariable = 0
	) const;

    //  Application Management
    private:
	void incrementApplicationCount ();
	void decrementApplicationCount ();

    //  Exit Status
    public:
	int getExitStatus () const {
	    return m_xExitStatus;
	}
	void setExitStatus (int xExitStatus);

    //  State
    protected:
	CommandLine const	m_iCommandLine;
	Reference const		m_pParent;
	counter_t		m_cApplications;
	int			m_xExitStatus;
    };
}


#endif
