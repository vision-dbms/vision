#ifndef Vca_VBSProducerConsumerDevice__Interface
#define Vca_VBSProducerConsumerDevice__Interface

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
    /**
     * Abstract device providing both read and write capabilities.
     *
     * @tparam Implementation delegate class used to provide device-specific implementation details.
     */
    template <class Implementation> class VBSProducerConsumerDevice_
	: public VDevice_<Implementation>
	, private VDevice::BSReadFace
	, private VDevice::BSWriteFace
    {
	DECLARE_CONCRETE_RTTLITE (VBSProducerConsumerDevice_<Implementation>, VDevice_<Implementation>);

    //  Aliases
    public:
	typedef typename Implementation::Data ImplementationData;
	typedef typename BaseClass::Get Get;
	typedef typename BaseClass::Put Put;
	typedef typename BaseClass::ReadPoll ReadPoll;
	typedef typename BaseClass::WritePoll WritePoll;

    //  Construction
    public:
	VBSProducerConsumerDevice_(ImplementationData &rDeviceData)
	    : BaseClass (rDeviceData), VDevice::BSReadFace (this), VDevice::BSWriteFace (this)
	{
	}

    //  Destruction
    private:
	~VBSProducerConsumerDevice_() {
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
	    VkStatus &rStatus, VDeviceBSReader *pUser, VDeviceBSReadArea const &rArea
	) {
	    VReference<Get> pUse (new Get (this));
	    return pUse->start (rStatus, pUser, rArea);
	}
	bool start_(
	    VkStatus &rStatus, VDeviceBSWriter *pUser, VDeviceBSWriteArea const &rArea
	) {
	    VReference<Put> pUse (new Put (this));
	    return pUse->start (rStatus, pUser, rArea);
	}
        bool start_(
	    VkStatus &rStatus, VDeviceBSReader *pUser
        ) {
	    VReference<ReadPoll> pUse (new ReadPoll (this));
	    return pUse->start (rStatus, pUser);
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
        operator BSReadFace* () { return this; }

    //  User Accounting
    private:
	void onFirstUser_(VDevice::BSReadFace *pFace) {
	    static_cast<Implementation*>(this)->onFirstReader ();
	}
	void onFinalUser_(VDevice::BSReadFace *pFace) {
	    static_cast<Implementation*>(this)->onFinalReader ();
	}

	void onFirstUser_(VDevice::BSWriteFace *pFace) {
	    static_cast<Implementation*>(this)->onFirstWriter ();
	}
	void onFinalUser_(VDevice::BSWriteFace *pFace) {
	    static_cast<Implementation*>(this)->onFinalWriter ();
	}

    //  User Creation
    private:
	bool supplyConnection_(VReference<VConnection>&rpUser) {
	    return static_cast<VDevice::BSReadFace*>(this)->supply (rpUser);
	}
	bool supplyBSProducer_(VReference<VBSProducer>&rpUser) {
	    return static_cast<VDevice::BSReadFace*>(this)->supply (rpUser);
	}
	bool supplyBSConsumer_(VReference<VBSConsumer>&rpUser) {
	    return static_cast<VDevice::BSWriteFace*>(this)->supply (rpUser);
	}
    };
}


#endif
