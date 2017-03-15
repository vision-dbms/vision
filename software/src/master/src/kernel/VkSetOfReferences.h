#ifndef VkSetOfReferences_Interface
#define VkSetOfReferences_Interface

/************************
 *****  Components  *****
 ************************/

#include "VkSetOf.h"

#include "VReference.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

template <class ERType> class VkSetOfReferences : public VkSetOf<VReference<ERType>,ERType*,ERType const*> {
};


#endif
