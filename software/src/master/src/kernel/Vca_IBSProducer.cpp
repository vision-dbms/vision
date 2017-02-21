/*****  Vca_IBSProducer Implementation  *****/

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

#define   Vca_IBSProducer
#include "Vca_IBSProducer.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_CompilerHappyPill.h"


/******************************
 ******************************
 *****                    *****
 *****  Vca::IBSProducer  *****
 *****                    *****
 ******************************
 ******************************/

VINTERFACE_TEMPLATE_EXPORTS (Vca::IBSProducer)

namespace Vca {
    // {1DD950C5-6DF0-4ae0-A2AB-882C7390623C}
    VINTERFACE_TYPEINFO_DEFINITION (
	IBSProducer,
	0x1dd950c5, 0x6df0, 0x4ae0, 0xa2, 0xab, 0x88, 0x2c, 0x73, 0x90, 0x62, 0x3c
    );

    /*************************
     *************************
     *****  Member Info  *****
     *************************
     *************************/

    VINTERFACE_MEMBERINFO_DEFINITION (IBSProducer, Get, 0);
}
