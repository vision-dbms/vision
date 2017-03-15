/*****  Vca_IActivity Implementation  *****/

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

#define   Vca_IActivity
#include "Vca_IActivity.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/****************************
 ****************************
 *****                  *****
 *****  Vca::IActivity  *****
 *****                  *****
 ****************************
 ****************************/

/***********************
 ***********************
 *****  Type Info  *****
 ***********************
 ***********************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IActivity)

namespace Vca {
// {BF89B4B7-C1A4-4467-A374-AB7EF8CF51C1}
    VINTERFACE_TYPEINFO_DEFINITION (
	IActivity,
	0xbf89b4b7, 0xc1a4, 0x4467, 0xa3, 0x74, 0xab, 0x7e, 0xf8, 0xcf, 0x51, 0xc1
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IActivity, SupplyEndErr	, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (IActivity, SupplyEvents	, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (IActivity, GetSSID	, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (IActivity, GetParent	, 3);
    VINTERFACE_MEMBERINFO_DEFINITION (IActivity, GetDescription	, 4);
}
