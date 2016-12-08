#ifndef Vca_VFilterDeviceFactory_Interface
#define Vca_VFilterDeviceFactory_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VRolePlayer.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class IPipeSource;
    class IPipeSourceClient;
    class VDevice;

    class Vca_API VFilterDeviceFactory : public VRolePlayer {
	DECLARE_ABSTRACT_RTTLITE (VFilterDeviceFactory, VRolePlayer);

    //  Construction
    protected:
	VFilterDeviceFactory ();

    //  Destruction
    protected:
	~VFilterDeviceFactory ();

    //  Local Use
    protected:
	void supplyClient (IPipeSourceClient *pClient, VDevice *pDevice) const;
    };
}


#endif
