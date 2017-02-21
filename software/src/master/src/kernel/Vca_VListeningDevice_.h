#ifndef Vca_VListeningDevice__Interface
#define Vca_VListeningDevice__Interface

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
    template <class Implementation> class VListeningDevice_
	: public VDevice_<Implementation>, private VDevice::ListenFace
    {
	DECLARE_CONCRETE_RTTLITE (VListeningDevice_<Implementation>, VDevice_<Implementation>);

    //  Aliases
    public:
	typedef typename Implementation::Data ImplementationData;
	typedef VAccept_<Implementation> Accept;

    //  Construction
    public:
	VListeningDevice_(ImplementationData &rDeviceData) : BaseClass (rDeviceData), VDevice::ListenFace (this) {
	}

    //  Destruction
    private:
	~VListeningDevice_() {
	}

    //  Face Implementation
    private:
	VDevice *device_() {
	    return this;
	}
	bool getName_(VkStatus &rStatus, VString &rName) {
	    return static_cast<BaseClass*>(this)->getName (rStatus, rName);
	}
	bool start_(VkStatus &rStatus, VDevice::Listener *pUser) {
	    VReference<Accept> pUse (new Accept (this));
	    return pUse->start (rStatus, pUser);
	}

    //  User Accounting
    private:
	void onFirstUser_(VDevice::ListenFace *pFace) {
	    static_cast<Implementation*>(this)->onFirstListener ();
	}
	void onFinalUser_(VDevice::ListenFace *pFace) {
	    static_cast<Implementation*>(this)->onFinalListener ();
	}

    //  User Creation
    private:
	bool supplyListener_(VReference<VListener>&rpUser) {
	    return static_cast<VDevice::ListenFace*>(this)->supply (rpUser);
	}
    };
}


#endif
