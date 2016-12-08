/*****  Vxa_INumericSink Implementation  *****/

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

#define   Vxa_INumericSink
#include "Vxa_INumericSink.h"

/************************
 *****  Supporting  *****
 ************************/


/*******************************
 *******************************
 *****                     *****
 *****  Vxa::INumericSink  *****
 *****                     *****
 *******************************
 *******************************/

VINTERFACE_TEMPLATE_EXPORTS (Vxa::INumericSink)

namespace Vxa {
// {CF8AFD4F-FA55-40d2-A21E-E1945851C995}
    VINTERFACE_TYPEINFO_DEFINITION (
	INumericSink,
	0xcf8afd4f, 0xfa55, 0x40d2, 0xa2, 0x1e, 0xe1, 0x94, 0x58, 0x51, 0xc9, 0x95
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnF32S, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnF64S, 1);

    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnI08S, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnI16S, 3);
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnI32S, 4);
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnI64S, 5);

    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnU08S, 6);
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnU16S, 7);
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnU32S, 8);
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnU64S, 9);

    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnF32A, 10);
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnF64A, 11);

    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnI08A, 12);
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnI16A, 13);
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnI32A, 14);
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnI64A, 15);

    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnU08A, 16);
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnU16A, 17);
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnU32A, 18);
    VINTERFACE_MEMBERINFO_DEFINITION (INumericSink, OnU64A, 19);
}
