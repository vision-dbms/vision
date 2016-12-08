#ifndef Vca_VBSConsumerDevice__Interface
#define Vca_VBSConsumerDevice__Interface

/*********************
 *****  Library  *****
 *********************/

#include "Vca.h"

/************************
 *****  Components  *****
 ************************/

#include "Vca_VDevice_.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    template <class Implementation> class VBSConsumerDevice_
	: public VDevice_<Implementation>, private VDevice::BSWriteFace
    {
	DECLARE_CONCRETE_RTTLITE (VBSConsumerDevice_<Implementation>, VDevice_<Implementation>);

    //  Aliases
    public:
	typedef typename Implementation::Data ImplementationData;
	typedef typename BaseClass::Put Put;
	typedef typename BaseClass::WritePoll WritePoll;

    //  Construction
    public:
	VBSConsumerDevice_(ImplementationData &rDeviceData) : BaseClass (rDeviceData), VDevice::BSWriteFace (this) {
	}

    //  Destruction
    private:
	~VBSConsumerDevice_() {
	}

    //  Face Implementation
    private:
	VDevice *device_() {
	    return this;
	}
	bool getName_(VkStatus &rStatus, VString &rName) {
	    return static_cast<BaseClass*>(this)->getName (rStatus, rName);
	}
	bool start_(
	    VkStatus &rStatus, VDeviceBSWriter *pUser, VDeviceBSWriteArea const &rArea
	) {
	    VReference<Put> pUse (new Put (this));
	    return pUse->start (rStatus, pUser, rArea);
	}
        bool start_(
	    VkStatus &rStatus, VDeviceBSWriter *pUser
        ) {
	    VReference<WritePoll> pUse (new WritePoll (this));
	    return pUse->start (rStatus, pUser);
        }

    //  Face Access
    public:
        operator BSWriteFace* () { return this; }

    //  User Accounting
    private:
	void onFirstUser_(VDevice::BSWriteFace *pFace) {
	    static_cast<Implementation*>(this)->onFirstWriter ();
	}
	void onFinalUser_(VDevice::BSWriteFace *pFace) {
	    static_cast<Implementation*>(this)->onFinalWriter ();
	}

    //  User Creation
    private:
	bool supplyConnection_(VReference<VConnection>&rpUser) {
	    return static_cast<VDevice::BSWriteFace*>(this)->supply (rpUser);
	}
	bool supplyBSConsumer_(VReference<VBSConsumer>&rpUser) {
	    return static_cast<VDevice::BSWriteFace*>(this)->supply (rpUser);
	}
    };
}


#endif
