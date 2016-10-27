#ifndef Vca_VPassiveDirectiveAgent_Interface
#define Vca_VPassiveDirectiveAgent_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VPassiveAgent.h"

#include "Vca_IPipeSource.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VInstanceOfInterface.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VPassiveDirectiveAgent : public VPassiveAgent {
	DECLARE_ABSTRACT_RTTLITE (VPassiveDirectiveAgent, VPassiveAgent);

    //  Aliases
    public:
	typedef VInstanceOfInterface<IPipeSource> PipeSourceInstance;
	typedef PipeSourceInstance::gofer_t PipeSourceGofer;

    //  Pipe Sink
    public:
	class PipeSink;
	friend class PipeSink;

    //  Construction
    protected:
	VPassiveDirectiveAgent (VString const &rDirectivePrefix);

    //  Destruction
    protected:
	~VPassiveDirectiveAgent ();

    //  Access
    public:
	void supplyDirective (VString &rDirective, CallData const &rCallData) const {
	    supplyDirective_(rDirective, m_iDirectivePrefix, rCallData);
	}
    private:
	virtual void supplyDirective_(
	    VString &rDirective, VString const &rDirectivePrefix, CallData const &rCallData
	) const = 0;
    public:
	bool supplyPassiveServerGofer (PassiveServerGofer::Reference &rpGofer, PipeSourceGofer *pPipeSourceGofer);

    //  Use
    protected:
	void processCallbackRequest (CallData const &rCallData);
    private:
	void getSuccessor (AgentReference &rpSuccessor) {
	    rpSuccessor.setTo (this);
	}

    //  Callbacks
    private:
	virtual void onPlumbing (
	    CallData const &rCallData, VBSConsumer *pStdBSToPeer, VBSProducer *pStdBSToHere, VBSProducer *pErrBSToHere
	);

    //  Temporary hack...
    protected:
	void startLineGrabber (VBSProducer *pBSProducer);
    public:
	bool onInputLine (char const *pLine, size_t sLine);
	void onInputDone ();

    //  State
    private:
	VString const m_iDirectivePrefix;
	PipeSourceGofer::Reference m_pPipeSourceGofer;
	static bool const g_bTracingLineGrabber;
    };
}


#endif /* Vca_VPassiveDirectiveAgent_Interface */
