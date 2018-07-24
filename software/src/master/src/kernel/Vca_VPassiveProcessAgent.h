#ifndef Vca_VPassiveProcessAgent_Interface
#define Vca_VPassiveProcessAgent_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VPassiveDirectiveAgent.h"

#include "Vca_VGoferExogenous.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class VPassiveProcessAgent : public VPassiveDirectiveAgent {
	DECLARE_CONCRETE_RTTLITE (VPassiveProcessAgent, VPassiveDirectiveAgent);

    //  Construction
    public:
	VPassiveProcessAgent (VString const &rDirectivePrefix);

    //  Destruction
    private:
	~VPassiveProcessAgent ();

    //  Access
    public:
	VGoferInstance_String *directiveGofer () const {
	    return m_pDirectiveGofer;
	}
    private:
	virtual void supplyDirective_(
	    VString &rDirective, VString const &rDirectivePrefix, CallData const &rCallData
	) const OVERRIDE;

    //  Use
    private:
	virtual void processCallbackRequest (CallData const &rCallData) OVERRIDE;

    //  State
    private:
	VGoferExogenous_String::Reference m_pDirectiveGofer;
    };
}


#endif /* Vca_VPassiveProcessAgent_Interface */
