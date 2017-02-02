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
	void supplyDirective_(
	    VString &rDirective, VString const &rDirectivePrefix, CallData const &rCallData
	) const;

    //  Use
    private:
	void processCallbackRequest (CallData const &rCallData);

    //  State
    private:
	VGoferExogenous_String::Reference m_pDirectiveGofer;
    };
}


#endif /* Vca_VPassiveProcessAgent_Interface */
