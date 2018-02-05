#ifndef VsaPrompt_Interface
#define VsaPrompt_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VClientApplication.h"

#include "Vca_VGoferInterface.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vsa_IEvaluator.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vsa {
    class VPromptApplication : public Vca::VClientApplication {
	DECLARE_CONCRETE_RTTLITE (VPromptApplication, Vca::VClientApplication);

    //  Aliases
    public:
	typedef Vca::VGoferInterface<IEvaluator> evaluator_gofer_t;

    //  Construction
    public:
	VPromptApplication (Context *pContext);

    //  Destruction
    private:
	~VPromptApplication ();

    //  Startup
    private:
	virtual bool start_() OVERRIDE;

    private:
	virtual bool getDefaultServerName (VString &rServerName) const OVERRIDE;

	void onGoferValue (IEvaluator *pIEvaluator);
	void onGoferError (Vca::IError* pInterface, VString const& rMessage) {
	    onConnectError (pInterface, rMessage);
	}

    //  State
    private:
	evaluator_gofer_t::Reference m_pEvaluatorGofer;
    };
}


#endif
