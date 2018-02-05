#ifndef Vca_VTriggerTicket_Interface
#define Vca_VTriggerTicket_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VTriggerableTicket.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class ITrigger;

    class Vca_API VTriggerTicket : public VTriggerableTicket {
	DECLARE_CONCRETE_RTTLITE (VTriggerTicket, VTriggerableTicket);

    //  Construction
    public:
	VTriggerTicket (
	    VReferenceable *pListOwner, List &rList, ITrigger *pTrigger, bool bTriggered = false
	);

    //  Destruction
    private:
	~VTriggerTicket ();

    //  Triggering
    private:
	void trigger () const OVERRIDE;

    //  State
    private:
	VReference<ITrigger> const m_pTrigger;
    };
}


#endif
