#ifndef Vca_VLoopbackDeviceFactory_Interface
#define Vca_VLoopbackDeviceFactory_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VStockDeviceFactory_.h"
#include "Vca_VLoopbackDeviceImplementation.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    typedef VStockDeviceFactory_<VLoopbackDeviceImplementation> VLoopbackDeviceFactory;
}


#endif
