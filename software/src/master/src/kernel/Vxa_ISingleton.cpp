/*****  Vxa_ISingleton Implementation  *****/
#define Vxa_ISingleton_Instantiations

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

#define   Vxa_ISingleton
#include "Vxa_ISingleton.h"

/************************
 *****  Supporting  *****
 ************************/

#include "Vca_VTypeInfoHolderForArrayOf.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_EXPORTS_AT_LEVEL (Vxa::ISingleton,2)


/*****************************
 *****************************
 *****                   *****
 *****  Vxa::ISingleton  *****
 *****                   *****
 *****************************
 *****************************/

namespace Vxa {
    // {41759CFE-6D5B-47aa-8061-9D93580D249C}
    VINTERFACE_TYPEINFO_DEFINITION (
	ISingleton, 0x41759cfe, 0x6d5b, 0x47aa, 0x80, 0x61, 0x9d, 0x93, 0x58, 0xd, 0x24, 0x9c
    );

    VINTERFACE_MEMBERINFO_DEFINITION (ISingleton, ExternalImplementation, 0);

    // {801828C0-D831-4f35-9DE0-19291C5FFE7A}
    Vca::VTypeInfoHolderFor<VkDynamicArrayOf<ISingleton::Reference> const&> const
    VTypeInfoHolderFor_VkDynamicArrayOf_ISingleton_Reference__const_r (
	0x801828c0, 0xd831, 0x4f35, 0x9d, 0xe0, 0x19, 0x29, 0x1c, 0x5f, 0xfe, 0x7a
    );

    // {E216CC48-F5AC-441b-8CA6-7167D98D062E}
    Vca::VTypeInfoHolderForArrayOf<ISingleton::Reference> const
    VTypeInfoHolderForSerializableArrayOf_ISingleton_Reference (
	0xe216cc48, 0xf5ac, 0x441b, 0x8c, 0xa6, 0x71, 0x67, 0xd9, 0x8d, 0x6, 0x2e
    );
}
