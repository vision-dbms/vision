/*****  Vxa_ICollection Implementation  *****/

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

#define   Vxa_ICollection
#include "Vxa_ICollection.h"

/************************
 *****  Supporting  *****
 ************************/


/******************************
 ******************************
 *****                    *****
 *****  Vxa::ICollection  *****
 *****                    *****
 ******************************
 ******************************/

VINTERFACE_TEMPLATE_EXPORTS_AT_LEVEL (Vxa::ICollection,2)

namespace Vxa {
// {0A20AF1A-8617-432f-8E04-02776536BE0D}
    VINTERFACE_TYPEINFO_DEFINITION (
	ICollection,
	0xa20af1a, 0x8617, 0x432f, 0x8e, 0x4, 0x2, 0x77, 0x65, 0x36, 0xbe, 0xd
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (ICollection, Bind  , 0);
    VINTERFACE_MEMBERINFO_DEFINITION (ICollection, Invoke, 1);
    VINTERFACE_MEMBERINFO_DEFINITION (ICollection, QueryCardinality, 2);
}
