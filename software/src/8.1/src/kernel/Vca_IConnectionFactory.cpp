/*****  Vca_IConnectionFactory Implementation  *****/

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

#define   Vca_IConnectionFactory
#include "Vca_IConnectionFactory.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/*************************************
 *************************************
 *****                           *****
 *****  Vca::IConnectionFactory  *****
 *****                           *****
 *************************************
 *************************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IConnectionFactory)

namespace Vca {
// {4FFC2160-E5B9-422b-8FAB-03AB60B1987C}
    VINTERFACE_TYPEINFO_DEFINITION (
	IConnectionFactory,
	0x4ffc2160, 0xe5b9, 0x422b, 0x8f, 0xab, 0x3, 0xab, 0x60, 0xb1, 0x98, 0x7c
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (IConnectionFactory, MakeConnection, 0);
}
