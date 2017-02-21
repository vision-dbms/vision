/*****  Vca_IActivitySource Implementation  *****/

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

#define   Vca_IActivitySource
#include "Vca_IActivitySource.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/**********************************
 **********************************
 *****                        *****
 *****  Vca::IActivitySource  *****
 *****                        *****
 **********************************
 **********************************/

/***********************
 ***********************
 *****  Type Info  *****
 ***********************
 ***********************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IActivitySource)

namespace Vca {
// {9A7915E9-D708-4b31-92DC-B10ADC444809}
    VINTERFACE_TYPEINFO_DEFINITION (
	IActivitySource,
	0x9a7915e9, 0xd708, 0x4b31, 0x92, 0xdc, 0xb1, 0xa, 0xdc, 0x44, 0x48, 0x9
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IActivitySource, SupplyActivities, 0);
}
