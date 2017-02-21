/*****  Vca_IDie Implementation  *****/

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

#define   Vca_Main_IDie
#include "Vca_IDie.h"

/************************
 *****  Supporting  *****
 ************************/


/***********************
 ***********************
 *****             *****
 *****  Vca::IDie  *****
 *****             *****
 ***********************
 ***********************/


VINTERFACE_TEMPLATE_EXPORTS (Vca::IDie)

namespace Vca {
// {AF909E26-9379-4239-AA68-09BB74FF070B}
    VINTERFACE_TYPEINFO_DEFINITION (
	IDie,
	0xaf909e26, 0x9379, 0x4239, 0xaa, 0x68, 0x9, 0xbb, 0x74, 0xff, 0x7, 0xb
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IDie, Die, 0);
}

