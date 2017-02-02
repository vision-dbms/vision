/*****  Vca_IProcess Implementation  *****/

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

#define   Vca_IProcess
#include "Vca_IProcess.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/***************************
 ***************************
 *****                 *****
 *****  Vca::IProcess  *****
 *****                 *****
 ***************************
 ***************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IProcess)

namespace Vca {
// {78F8C4C1-9563-4ba1-8D66-AB8866F687FC}
    VINTERFACE_TYPEINFO_DEFINITION (
	IProcess,
	0x78f8c4c1, 0x9563, 0x4ba1, 0x8d, 0x66, 0xab, 0x88, 0x66, 0xf6, 0x87, 0xfc
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IProcess, WaitForTermination, 0);
}
