#ifndef Vca_IPipeSource_Gofers_Interface
#define Vca_IPipeSource_Gofers_Interface

#ifndef Vca_IPipeSource_Gofers
#define Vca_IPipeSource_Gofers extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vca_VGoferInterface.h"

#include "Vca_IPipeSource.h"

#include "Vca_VInstanceOf.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    DECLARE_GOFER_INTERFACE (Vca_IPipeSource_Gofers, Vca_API, IPipeSource)

    namespace IPipeSource_Gofer {
	typedef VGoferInterface<IPipeSource> gofer_base_t;

    /*********************************************************
     *----  class Vca::IPipeSource_Gofer::NewConnection  ----*
     *********************************************************/

	class Vca_API NewConnection : public gofer_base_t {
	    DECLARE_CONCRETE_RTTLITE (NewConnection,gofer_base_t);

	//  Construction
	public:
	    template <typename address_t> NewConnection (
		address_t iAddress, bool bAServerFileName
	    ) : m_iAddress (iAddress), m_bAServerFileName (bAServerFileName) {
	    }

	//  Destruction
	private:
	    ~NewConnection ();

	//  Callbacks
	private:
	    virtual void onNeed () OVERRIDE {
		m_iAddress.materializeFor (this);
		BaseClass::onNeed ();
	    }
	    virtual void onData () OVERRIDE;

	//  State
	private:
	    VInstanceOf_String m_iAddress;
	    bool const m_bAServerFileName;
	};

    /******************************************************
     *----  class Vca::IPipeSource_Gofer::NewProcess  ----*
     ******************************************************/

	class Vca_API NewProcess : public gofer_base_t {
	    DECLARE_CONCRETE_RTTLITE (NewProcess,gofer_base_t);

	//  Construction
	public:
	    template <
		typename command_t, typename plumbing_stdin_t, typename plumbing_stdout_t, typename plumbing_stderr_t
	    > NewProcess (
		command_t iCommand, plumbing_stdin_t bPlumbingStdin, plumbing_stdout_t bPlumbingStdout, plumbing_stderr_t bPlumbingStderr
	    ) : m_iCommand (iCommand), m_bPlumbingStdin (bPlumbingStdin), m_bPlumbingStdout (bPlumbingStdout), m_bPlumbingStderr (bPlumbingStderr) {
	    }
	    template <typename command_t> NewProcess (
		command_t iCommand
	    ) : m_iCommand (iCommand), m_bPlumbingStdin (true), m_bPlumbingStdout (true), m_bPlumbingStderr (true) {
	    }

	//  Destruction
	private:
	    ~NewProcess ();

	//  Callbacks
	private:
	    virtual void onNeed () OVERRIDE {
		m_iCommand.materializeFor (this);
		m_bPlumbingStdin.materializeFor (this);
		m_bPlumbingStdout.materializeFor (this);
		m_bPlumbingStderr.materializeFor (this);
		BaseClass::onNeed ();
	    }
	    virtual void onData () OVERRIDE;

	//  State
	private:
	    VInstanceOf_String m_iCommand;
	    VInstanceOf_bool m_bPlumbingStdin;
	    VInstanceOf_bool m_bPlumbingStdout;
	    VInstanceOf_bool m_bPlumbingStderr;
	};

    /****************************************************
     *----  class Vca::IPipeSource_Gofer::NewRexec  ----*
     ****************************************************/

	class Vca_API NewRexec : public gofer_base_t {
	    DECLARE_CONCRETE_RTTLITE (NewRexec,gofer_base_t);

	//  Construction
	public:
	    template <
		typename host_t, typename login_t, typename password_t, typename command_t, typename plumbing_t
	    > NewRexec (host_t iHost, login_t iLogin, password_t iPassword, command_t iCommand, plumbing_t b3Pipe)
		: m_iHost (iHost)
		, m_iLogin (iLogin)
		, m_iPassword (iPassword)
		, m_iCommand (iCommand)
		, m_b3Pipe (b3Pipe)
	    {
	    }

	//  Destruction
	private:
	    ~NewRexec ();

	//  Callbacks
	private:
	    virtual void onNeed () OVERRIDE {
		m_iHost.materializeFor (this);
		m_iLogin.materializeFor (this);
		m_iPassword.materializeFor (this);
		m_iCommand.materializeFor (this);
		m_b3Pipe.materializeFor (this);
		BaseClass::onNeed ();
	    }
	    virtual void onData () OVERRIDE;

	//  State
	private:
	    VInstanceOf_String	m_iHost;
	    VInstanceOf_String	m_iLogin;
	    VInstanceOf_String	m_iPassword;
	    VInstanceOf_String	m_iCommand;
	    VInstanceOf_bool	m_b3Pipe;
	};

    /**************************************************
     *----  class Vca::IPipeSource_Gofer::NewSSH  ----*
     **************************************************/

	class Vca_API NewSSH : public gofer_base_t {
	    DECLARE_CONCRETE_RTTLITE (NewSSH,gofer_base_t);

	//  Construction
	public:
	    template <
		typename host_t, typename login_t, typename password_t, typename command_t, typename plumbing_t
	    > NewSSH (host_t iHost, login_t iLogin, password_t iPassword, command_t iCommand, plumbing_t b3Pipe)
		: m_iHost (iHost)
		, m_iLogin (iLogin)
		, m_iPassword (iPassword)
		, m_iCommand (iCommand)
		, m_b3Pipe (b3Pipe)
	    {
	    }

	//  Destruction
	private:
	    ~NewSSH ();

	//  Callbacks
	private:
	    virtual void onNeed () OVERRIDE {
		m_iHost.materializeFor (this);
		m_iLogin.materializeFor (this);
		m_iPassword.materializeFor (this);
		m_iCommand.materializeFor (this);
		m_b3Pipe.materializeFor (this);
		BaseClass::onNeed ();
	    }
	    virtual void onData () OVERRIDE;

	//  State
	private:
	    VInstanceOf_String	m_iHost;
	    VInstanceOf_String	m_iLogin;
	    VInstanceOf_String	m_iPassword;
	    VInstanceOf_String	m_iCommand;
	    VInstanceOf_bool	m_b3Pipe;
	};
    }
}

#endif
