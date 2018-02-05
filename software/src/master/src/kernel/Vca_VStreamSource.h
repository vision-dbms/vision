#ifndef Vca_VStreamSource_Interface
#define Vca_VStreamSource_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VBSProducer.h"
#include "Vca_VDevice.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

/***************************
 *----  VStreamSource  ----*
 ***************************/

namespace Vca {
    /**
     * A VBSProducer concrete subclass that implemented via a device's BSReader.
     */
    class VStreamSource : public VBSProducer, private VDevice::BSReader {
	DECLARE_CONCRETE_RTTLITE (VStreamSource, VBSProducer);

    //  Device Agent
    public:
	typedef VDeviceBSReadFace	DeviceFace;
	typedef VDeviceBSReader		DeviceUser;

    //  Construction
    public:
	VStreamSource (DeviceFace *pDeviceFace);

    //  Destruction
    private:
	~VStreamSource ();

    //  Access
    public:
	size_t GetByteCount () OVERRIDE;

    //  Callback
    private:
	void trigger_() OVERRIDE {
	    onGetContinuation ();
	}

    //  Completion
    private:
	void onTransfer_(size_t sTransfer) OVERRIDE {
	    onGetCompletion (sTransfer);
	}

    //  Control
    private:
	void endTransfers () OVERRIDE;

    //  Data Transfer
    private:
	void transferData () OVERRIDE;
    };
}


#endif
