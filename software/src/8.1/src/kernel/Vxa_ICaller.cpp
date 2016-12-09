/*****  Vxa_ICaller Implementation  *****/

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

#define   Vxa_ICaller
#include "Vxa_ICaller.h"

/************************
 *****  Supporting  *****
 ************************/


/**************************
 **************************
 *****                *****
 *****  Vxa::ICaller  *****
 *****                *****
 **************************
 **************************/

VINTERFACE_TEMPLATE_EXPORTS (Vxa::ICaller)

namespace Vxa {
// {D87C5A8A-5FF1-45ec-849B-A95E5A924CCB}
    VINTERFACE_TYPEINFO_DEFINITION (
	ICaller,
	0xd87c5a8a, 0x5ff1, 0x45ec, 0x84, 0x9b, 0xa9, 0x5e, 0x5a, 0x92, 0x4c, 0xcb
    );

//  Member Definitions
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, GetSelfReference, 0);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, GetSelfReferences, 1);

    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnObjectReference, 2);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnObjectReferences, 3);

    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnS08Array, 4);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnU08Array, 5);

    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnS16Array, 6);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnU16Array, 7);

    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnS32Array, 8);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnU32Array, 9);

    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnS64Array, 10);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnU64Array, 11);

    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnNA, 12);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnNASegment, 13);

    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnBooleanSegment, 14);

    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnS08Segment, 15);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnU08Segment, 16);

    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnS16Segment, 17);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnU16Segment, 18);

    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnS32Segment, 19);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnU32Segment, 20);

    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnS64Segment, 21);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnU64Segment, 22);

    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnF32Segment, 23);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnF64Segment, 24);

    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnStringSegment, 25);
    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnObjectSegment, 26);

    VINTERFACE_MEMBERINFO_DEFINITION (ICaller, ReturnSegmentCount, 27);
}
