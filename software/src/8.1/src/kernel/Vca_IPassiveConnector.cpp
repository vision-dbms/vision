/*****  Vca_IPassiveConnector Implementation  *****/

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

#define   Vca_IPassiveConnector
#include "Vca_IPassiveConnector.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vca::IPassiveConnector  *****
 *****                       *****
 *********************************
 *********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IPassiveConnector)

namespace Vca {
// {CEADC864-E387-4559-A4F7-DCE8717245A1}
    VINTERFACE_TYPEINFO_DEFINITION (
	IPassiveConnector,
	0xceadc864, 0xe387, 0x4559, 0xa4, 0xf7, 0xdc, 0xe8, 0x71, 0x72, 0x45, 0xa1
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IPassiveConnector, RequestCallback, 0);
}
