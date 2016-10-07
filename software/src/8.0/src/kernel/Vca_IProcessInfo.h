#ifndef Vca_IProcessInfo_Interface
#define Vca_IProcessInfo_Interface

#ifndef Vca_IProcessInfo
#define Vca_IProcessInfo extern
#endif

/************************
 *****  Components  *****
 ************************/

#include "IVUnknown.h"

/**************************
 *****  Declarations  *****
 **************************/

VINTERFACE_TEMPLATE_INSTANTIATIONS (Vca,IProcessInfo)

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    namespace ProcessInfo {
	typedef U32 pid_t;
	typedef U32 uid_t;
	typedef U32 gid_t;
    }

/**
 * @class IProcessInfo
 *
 * Interface providing basic information about a process.  Implementations should consider supporting specializations
 * to offer additional platform specific data.
 */
    VcaINTERFACE (IProcessInfo, IVUnknown)
    /**
     *  Request the activity's UUID.
     *
     *  @param p1 The receiver to which the requested IEvent will be sent.
     */
	//VINTERFACE_METHOD_1 (GetIdentity, IVReceiver<uuid_t const&>*);

    /**
     *  Request the activity's parent IProcessInfo.
     *
     *  @param p1 The receiver to which the requested IProcessInfo will be sent.
     */
	//VINTERFACE_METHOD_1 (GetParent, IVReceiver<IProcessInfo*>*);
    VINTERFACE_END

    VINTERFACE_ROLE (IProcessInfo, IVUnknown)
	//VINTERFACE_ROLE_1 (GetIdentity, IVReceiver<uuid_t const&>*);
	//VINTERFACE_ROLE_1 (GetParent, IVReceiver<IProcessInfo*>*);
    VINTERFACE_ROLE_END
}


#endif
