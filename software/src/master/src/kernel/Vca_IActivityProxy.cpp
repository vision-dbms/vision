/*****  Vca_IActivityProxy Implementation  *****/

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

#define   Vca_IActivityProxy
#include "Vca_IActivityProxy.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vca::IActivityProxy  *****
 *****                       *****
 *********************************
 *********************************/

/***********************
 ***********************
 *****  Type Info  *****
 ***********************
 ***********************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IActivityProxy)

namespace Vca {
// {1600FE03-ABDF-4088-9687-F0F1C98AB585}
    VINTERFACE_TYPEINFO_DEFINITION (
	IActivityProxy,
	0x1600fe03, 0xabdf, 0x4088, 0x96, 0x87, 0xf0, 0xf1, 0xc9, 0x8a, 0xb5, 0x85
    );

//  Member Definitions
//    VINTERFACE_MEMBERINFO_DEFINITION (IActivityProxy, GetMaster, 0);
}
