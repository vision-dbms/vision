/*****  Vca_IProcessInfo Implementation  *****/

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

#define   Vca_IProcessInfo
#include "Vca_IProcessInfo.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*******************************
 *******************************
 *****                     *****
 *****  Vca::IProcessInfo  *****
 *****                     *****
 *******************************
 *******************************/

/***********************
 ***********************
 *****  Type Info  *****
 ***********************
 ***********************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IProcessInfo)

namespace Vca {
// {AE8406CF-473E-4bb5-9AFD-6D6236434CC8}
    VINTERFACE_TYPEINFO_DEFINITION (
	IProcessInfo,
	0xae8406cf, 0x473e, 0x4bb5, 0x9a, 0xfd, 0x6d, 0x62, 0x36, 0x43, 0x4c, 0xc8
    );

//  Member Definitions
    //VINTERFACE_MEMBERINFO_DEFINITION (IProcessInfo, GetIdentity, 0);
    //VINTERFACE_MEMBERINFO_DEFINITION (IProcessInfo, GetParent, 1);
}
