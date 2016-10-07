/*****  Vca_IStdPipeSource Implementation  *****/

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

#define   Vca_IStdPipeSource
#include "Vca_IStdPipeSource.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*********************************
 *********************************
 *****                       *****
 *****  Vca::IStdPipeSource  *****
 *****                       *****
 *********************************
 *********************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IStdPipeSource)

namespace Vca {
// {A2CC4794-9FCA-444b-A436-FC1557602A89}
    VINTERFACE_TYPEINFO_DEFINITION (
	IStdPipeSource,
	0xa2cc4794, 0x9fca, 0x444b, 0xa4, 0x36, 0xfc, 0x15, 0x57, 0x60, 0x2a, 0x89
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IStdPipeSource, Supply, 0);
}
