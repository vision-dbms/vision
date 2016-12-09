/*****  Vca_IEvent Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#define   Vca_IEvent
#include "Vca_IEvent.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*************************
 *************************
 *****               *****
 *****  Vca::IEvent  *****
 *****               *****
 *************************
 *************************/

/***********************
 ***********************
 *****  Type Info  *****
 ***********************
 ***********************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IEvent)

namespace Vca {
// {8909B284-26BA-45f6-9A5C-65E45840D846}
    VINTERFACE_TYPEINFO_DEFINITION (
	IEvent,
	0x8909b284, 0x26ba, 0x45f6, 0x9a, 0x5c, 0x65, 0xe4, 0x58, 0x40, 0xd8, 0x46
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IEvent, GetDescription	, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvent, GetTimestamp	, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IEvent, GetSSID		, 2);
}
