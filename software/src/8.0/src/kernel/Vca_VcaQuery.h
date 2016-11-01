#ifndef VcaQueryAgent_Interface
#define VcaQueryAgent_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VClientApplication.h"

#include "Vsa_VEvaluatorClient.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    class IEvaluator;

    class VQueryApp : public Vca::VClientApplication, public VEvaluatorClient {
	DECLARE_CONCRETE_RTTLITE (VQueryApp, Vca::VClientApplication);

    //  Construction
    public:
	VQueryApp (Context *pContext, argv_t envp);

    private:
	VString getPathInfo ();
	VString getQueryString ();

	static VString const &getInput ();

    //  Destruction
    protected:
	~VQueryApp () {
	}

    //  Query
    protected:
	bool isPOST () const {
	    return strcasecmp (EnvironmentString ("REQUEST_METHOD",""), "post") == 0;
	}

	bool isGET () const {
	    return strcasecmp (EnvironmentString ("REQUEST_METHOD",""), "get") == 0;
	}

    //  Startup
    private:
	bool start_();
	bool getDefaultServerName (VString &rServerName) const;

    //  VEvaluatorClient Callbacks
    public:
	void OnAccept_(IEvaluation *pEvaluation, Vca::U32 xQueuePosition);
        void OnResult_(IEvaluationResult*, VString const&);

	void OnError_(Vca::IError*, VString const&);

    //  Evaluator Receiver
    private:
	void onEvaluator (IEvaluator *pEvaluator);
	void onConnectionError (Vca::IError *pInterface, VString const &rMessage);

    //  Data Output
    private:
	unsigned int wrighte (int fd, char const *pData, unsigned int sData) const;

    public:
	void report (char const *pMessage) const;
    
    //  Static utilities
    private:
	static char const *EnvironmentString (char const *pVariable, char const *pDefaultValue);
	static VString const &Environment (argv_t ppEnvironment);

    //  State
    private:
	CommandCursor	m_iCommandCursor;
	VString	const	m_iPathInfo;	//  must follow m_iCommandCursor
	VString	const	m_iQueryString;	//  must follow m_iPathInfo
	VString const	m_iEnvironment;
        bool            m_bUsingAnyData;
    };
}

#endif
