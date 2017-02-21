#ifndef Vca_VFilterDeviceFactory__Interface
#define Vca_VFilterDeviceFactory__Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VFilterDeviceFactory.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "Vca_VBSProducerConsumerDevice_.h"

#include "Vca_VcaThreadState.h"

#include "Vca_VFilterPipeSource_.h"
#include "Vca_VFilterPipeSourceTransaction_.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template<class DeviceImplementation> class VFilterDeviceFactory_ : public VFilterDeviceFactory {
	DECLARE_CONCRETE_RTTLITE (VFilterDeviceFactory_<DeviceImplementation>, VFilterDeviceFactory);

    //  Device
    public:
	typedef VBSProducerConsumerDevice_<DeviceImplementation> Device;

    //  Factory Data
    public:
	typedef typename DeviceImplementation::FactoryData FactoryData;		//  core data + pipe source

    //  Pipe Source Data
    public:
	typedef typename DeviceImplementation::PipeSourceData PipeSourceData;	//  core data + filtered byte streams

    //  Pipe Source
    public:
	typedef VFilterPipeSource_<ThisClass> PipeSource;

    //  Pipe Source Transaction
    public:
	typedef VFilterPipeSourceTransaction_<ThisClass> PipeSourceTransaction;

    //  Construction
    private:
	VFilterDeviceFactory_() {
	    joinCohortAs (CohortIndex ());
	}

    //  Destruction
    private:
	~VFilterDeviceFactory_() {
	    exitCohortAs (CohortIndex ());
	}

    //  Cohort Index
    private:
	static VCohortIndex const &CohortIndex () {
	    static VCohortIndex iIndex;
	    return iIndex;
	}

    //  Cohort Instance
    public:
	static bool Supply (VReference<ThisClass>&rpInstance) {
	    rpInstance.setTo (
		reinterpret_cast<ThisClass*> (VcaThreadState::CohortValue (CohortIndex ()))
	    );
	    if (rpInstance.isNil ())
		rpInstance.setTo (new ThisClass ());
	    return rpInstance.isntNil ();
	}

    //  Local Use
    public:
	void supply (VReference<Device>&rpDevice, FactoryData &rData) {
	    rpDevice.setTo (new Device (rData));
	}
	void supply (IPipeSourceClient *pClient, FactoryData &rData) {
	    VReference<Device> pDevice;
	    supply (pDevice, rData);
	    supplyClient (pClient, pDevice);
	}

	void supply (
	    IPipeSourceClient *pClient, PipeSourceData const &rData, VBSProducer *pBSProducer, VBSConsumer *pBSConsumer
	) {
	    FactoryData iData (rData, pBSProducer, pBSConsumer);
	    supply (pClient, iData);
	}
	void supply (IPipeSourceClient *pClient, PipeSourceData const &rData, IPipeSource *pPipeSource) {
	    if (pClient) {
		VReference<PipeSourceTransaction> pTransaction (new PipeSourceTransaction (pClient, this, rData, pPipeSource));
	    }
	}
	void supply (VReference<PipeSource>&rpPipeSource, PipeSourceData const &rData, IPipeSource *pPipeSource) {
	    rpPipeSource.setTo (new PipeSource (this, rData, pPipeSource));
	}
    };
}


#endif
