#ifndef Vca_VStockDeviceFactory__Interface
#define Vca_VStockDeviceFactory__Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VStockDeviceFactory.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VBSProducerConsumerDevice_.h"
#include "Vca_VcaThreadState.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template <class DeviceImplementation> class VStockDeviceFactory_ : public VStockDeviceFactory {
	DECLARE_CONCRETE_RTTLITE (VStockDeviceFactory_<DeviceImplementation>, VStockDeviceFactory);

    //  Construction
    private:
	VStockDeviceFactory_ () {
	    joinCohortAs (CohortIndex ());
	}

    //  Destruction
    private:
	~VStockDeviceFactory_ () {
	    exitCohortAs (CohortIndex ());
	}

    //  Cohort Index
    private:
	static VCohortIndex const &CohortIndex ();

    //  Cohort Instance
    public:
	static bool Supply (Reference &rpInstance);

    //  Local Use
    private:
	virtual bool supply_(VDevice::Reference &rpDevice) {
	    DeviceImplementation iDD (this);
	    rpDevice.setTo (new VBSProducerConsumerDevice_<DeviceImplementation> (iDD));
	    return rpDevice.isntNil ();
	}
    };

    template <class DeviceImplementation> VCohortIndex const &VStockDeviceFactory_<DeviceImplementation>::CohortIndex () {
	static VCohortIndex iIndex;
	return iIndex;
    }

    template <class DeviceImplementation> bool VStockDeviceFactory_<DeviceImplementation>::Supply (Reference &rpInstance) {
	rpInstance.setTo (
	    reinterpret_cast<ThisClass*> (VcaThreadState::CohortValue (CohortIndex ()))
	);
	if (rpInstance.isNil ())
	    rpInstance.setTo (new ThisClass ());
	return rpInstance.isntNil ();
    }
}


#endif
