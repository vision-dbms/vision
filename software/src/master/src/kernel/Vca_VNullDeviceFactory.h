#ifndef Vca_VNullDeviceFactory_Interface
#define Vca_VNullDeviceFactory_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VStockDeviceFactory_.h"
#include "Vca_VNullDeviceImplementation.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    typedef VStockDeviceFactory_<VNullDeviceImplementation> VNullDeviceFactory;
}


#endif
