#ifndef Vca_VTriggerSet_Interface
#define Vca_VTriggerSet_Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "VkSetOfReferences.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_ITrigger.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class ITrigger;

    class Vca_API VTriggerSet : public VkSetOfReferences<ITrigger> {
	DECLARE_FAMILY_MEMBERS (VTriggerSet, VkSetOfReferences<ITrigger> );

    //  Construction
    public:
	VTriggerSet ();

    //  Destruction
    public:
	~VTriggerSet ();

    //  Use
    public:
	void trigger ();
        void removeAndTrigger ();

    //  State
    protected:
    };
}


#endif
