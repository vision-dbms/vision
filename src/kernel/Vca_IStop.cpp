/*****  Vca_IStop Implementation  *****/

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

#define   Vca_Main_IStop
#include "Vca_IStop.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/************************
 ************************
 *****              *****
 *****  Vca::IStop  *****
 *****              *****
 ************************
 ************************/


VINTERFACE_TEMPLATE_EXPORTS (Vca::IStop)

namespace Vca {
// {086207CC-AD82-4904-9A07-093468FA6183}
    VINTERFACE_TYPEINFO_DEFINITION (
	IStop,
	0x86207cc, 0xad82, 0x4904, 0x9a, 0x7, 0x9, 0x34, 0x68, 0xfa, 0x61, 0x83
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IStop, Stop, 0);
}
