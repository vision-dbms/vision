#ifndef Vca_VStreamSink_Interface
#define Vca_VStreamSink_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VBSConsumer.h"
#include "Vca_VDevice.h"

/**************************
 *****  Declarations  *****
 **************************/


/*************************
 *****  Definitions  *****
 *************************/

/*************************
 *----  VStreamSink  ----*
 *************************/

namespace Vca {
    class VStreamSink : public VBSConsumer, private VDevice::BSWriter {
	DECLARE_CONCRETE_RTTLITE (VStreamSink, VBSConsumer);

    //  Device Agent
    public:
	typedef VDeviceBSWriteFace	DeviceFace;
	typedef VDeviceBSWriter		DeviceUser;

    //  Construction
    public:
	VStreamSink (DeviceFace *pDeviceFace);

    //  Destruction
    private:
	~VStreamSink ();

    //  Callback
    private:
	virtual void trigger_() OVERRIDE {
	    onPutContinuation ();
	}

    //  Completion
    private:
	virtual void onTransfer_(size_t sTransfer) OVERRIDE {
	    onPutCompletion (sTransfer);
	}

    //  Control
    private:
	virtual void endTransfers () OVERRIDE;

    //  Data Transfer
    private:
	virtual void transferData () OVERRIDE;
    };
}


#endif
