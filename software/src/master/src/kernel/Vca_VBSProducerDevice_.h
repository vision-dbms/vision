#ifndef Vca_VBSProducerDevice__Interface
#define Vca_VBSProducerDevice__Interface

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
    template <class Implementation> class VBSProducerDevice_
	: public VDevice_<Implementation>, private VDevice::BSReadFace
    {
	DECLARE_CONCRETE_RTTLITE (VBSProducerDevice_<Implementation>, VDevice_<Implementation>);

    //  Aliases
    public:
	typedef typename Implementation::Data ImplementationData;
	typedef typename BaseClass::Get Get;
	typedef typename BaseClass::ReadPoll ReadPoll;

    //  Construction
    public:
	VBSProducerDevice_(ImplementationData &rDeviceData) : BaseClass (rDeviceData), VDevice::BSReadFace (this) {
	}

    //  Destruction
    protected:
	~VBSProducerDevice_() {
	}

    //  Face Implementation
    private:
	VDevice *device_() OVERRIDE {
	    return this;
	}
	bool getName_(VkStatus &rStatus, VString &rName) OVERRIDE {
	    return static_cast<BaseClass*>(this)->getName (rStatus, rName);
	}
	bool start_(
	    VkStatus &rStatus, VDeviceBSReader *pUser, VDeviceBSReadArea const &rArea
	) OVERRIDE {
	    VReference<Get> pUse (new Get (this));
	    return pUse->start (rStatus, pUser, rArea);
	}
        bool start_(
	    VkStatus &rStatus, VDeviceBSReader *pUser
        ) OVERRIDE {
	    VReference<ReadPoll> pUse (new ReadPoll (this));
	    return pUse->start (rStatus, pUser);
        }

    //  Face Access
    public:
        operator BSReadFace* () OVERRIDE { return this; }

    //  User Accounting
    private:
	void onFirstUser_(VDevice::BSReadFace *pFace) OVERRIDE {
	    static_cast<Implementation*>(this)->onFirstReader ();
	}
	void onFinalUser_(VDevice::BSReadFace *pFace) OVERRIDE {
	    static_cast<Implementation*>(this)->onFinalReader ();
	}

    //  User Creation
    private:
	bool supplyConnection_(VReference<VConnection>&rpUser) OVERRIDE {
	    return static_cast<VDevice::BSReadFace*>(this)->supply (rpUser);
	}
	bool supplyBSProducer_(VReference<VBSProducer>&rpUser) OVERRIDE {
	    return static_cast<VDevice::BSReadFace*>(this)->supply (rpUser);
	}
    };
}


#endif
