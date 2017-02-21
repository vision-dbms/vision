#ifndef Vxa_ICollection_Interface
#define Vxa_ICollection_Interface

#ifndef Vxa_ICollection
#define Vxa_ICollection extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "Vxa_ISingleton.h"

/**************************
 *****  Declarations  *****
 **************************/

DECLARE_VxaINTERFACE (ICaller)

VINTERFACE_TEMPLATE_INSTANTIATIONS_AT_LEVEL (Vxa,ICollection,2)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vxa {

//  Bind   (pCaller, rMessageName, cParameters, cTask); // Intention form
//  Invoke (pCaller, rMessageName, cParameters, cTask); // Extension form

    VINTERFACE_ROLE (ICollection, ISingleton)
	VINTERFACE_ROLE_4 (Bind  , ICaller*, VString const&, cardinality_t, cardinality_t);
	VINTERFACE_ROLE_4 (Invoke, ICaller*, VString const&, cardinality_t, cardinality_t);
	VINTERFACE_ROLE_1 (QueryCardinality, IVReceiver<cardinality_t>*);
    VINTERFACE_ROLE_END

    VxaINTERFACE (ICollection, ISingleton);
	VINTERFACE_METHOD_4 (Bind  , ICaller*, VString const&, cardinality_t, cardinality_t);
	VINTERFACE_METHOD_4 (Invoke, ICaller*, VString const&, cardinality_t, cardinality_t);
	VINTERFACE_METHOD_1 (QueryCardinality, IVReceiver<cardinality_t>*);
    VINTERFACE_END
}


#endif
