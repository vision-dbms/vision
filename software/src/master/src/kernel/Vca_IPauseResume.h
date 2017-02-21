#ifndef Vca_IPauseResume_Interface
#define Vca_IPauseResume_Interface

#ifndef Vca_Main_IPauseResume
#define Vca_Main_IPauseResume extern
#endif

/*********************
 *****  Library  *****
 *********************/

#include "Vca_Main.h"

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/*************************
 *****  Definitions  *****
 *************************/

//  pIPauseResume->Pause  ()
//  pIPauseResume->Resume ()

VINTERFACE_TEMPLATE_INSTANTIATIONS_IN_API(Vca_Main,Vca,IPauseResume)

#ifndef Vca_ITrigger
DECLARE_VcaINTERFACE (ITrigger)
#endif

namespace Vca {

    VINTERFACE_ROLE (IPauseResume, IVUnknown)
	VINTERFACE_ROLE_0 (Pause)
	VINTERFACE_ROLE_0 (Resume)
    VINTERFACE_ROLE_END

    VcaMainINTERFACE (IPauseResume, IVUnknown)
	VINTERFACE_METHOD_0 (Pause)
	VINTERFACE_METHOD_0 (Resume)
    VINTERFACE_END
}

#endif
