#ifndef VkSetOfFactoryReferences_Interface
#define VkSetOfFactoryReferences_Interface

/************************
 *****  Components  *****
 ************************/

#include "VkSetOf.h"

#include "VFactoryReference.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

template <class ReferentType> class VkSetOfFactoryReferences : public VkSetOf<
    VFactoryReference<ReferentType>,
    typename ReferentType::Initializer,
    typename ReferentType::Key
> {
};


#endif
