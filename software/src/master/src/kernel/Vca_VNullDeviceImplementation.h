#ifndef Vca_VNullDeviceImplementation_Interface
#define Vca_VNullDeviceImplementation_Interface

/************************
 *****  Components  *****
 ************************/

#include "Vca_VDeviceImplementation.h"

#include "V_VPointer.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace Vca {
    class Vca_API VNullDeviceImplementation : public VDeviceImplementation {
	DECLARE_FAMILY_MEMBERS (VNullDeviceImplementation, VDeviceImplementation );

    //  Data
    public:
	typedef ThisClass Data;

    //  Device Reference
    public:
	typedef VReference<ThisClass> DeviceReference;

    //  Use
    public:
	class Use : public VDeviceImplementation::AbstractUse {
	    DECLARE_FAMILY_MEMBERS (Use, VDeviceImplementation::AbstractUse);

	//  Friends
	    friend class VNullDeviceImplementation;

	//  Reference
	public:
	    typedef VReference<ThisClass> Reference;

	//  Construction
	protected:
	    Use (VReferenceable *pContainer, VNullDeviceImplementation *pDevice) : BaseClass (pContainer), m_pDevice (pDevice) {
	    }

	//  Destruction
	protected:
	    ~Use () {
	    }

	//  Access
	public:
	    VNullDeviceImplementation *device () const {
		return m_pDevice;
	    }

	//  State
	protected:
	    DeviceReference const m_pDevice;
	};
	friend class Use;

    //  BSGet
    public:
	class BSGet : public Use {
	    DECLARE_FAMILY_MEMBERS (BSGet, Use);

	    friend class VNullDeviceImplementation;

	//  Area
	public:
	    typedef VDeviceBSReadArea Area;

	//  Construction
	protected:
	    BSGet (VReferenceable *pContainer, VNullDeviceImplementation *pDevice) : BaseClass (pContainer, pDevice) {
	    }

	//  Destruction
	protected:
	    ~BSGet () {
	    }

	//  Startup
	public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	    bool start (VkStatus &rStatus, Area const &rArea) {
		//  Reads from the Null device always return EOF
		rStatus.MakeClosedStatus ();
		return false;
	    }
	};
	friend class BSGet;

    //  BSPut
    public:
	class BSPut : public Use {
	    DECLARE_FAMILY_MEMBERS (BSPut, Use);

	    friend class VNullDeviceImplementation;

	//  Area
	public:
	    typedef VDeviceBSWriteArea Area;

	//  Construction
	protected:
	    BSPut (VReferenceable *pContainer, VNullDeviceImplementation *pDevice) : BaseClass (pContainer, pDevice) {
	    }

	//  Destruction
	protected:
	    ~BSPut () {
	    }

	//  Startup
	public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	    bool start (VkStatus &rStatus, Area const &rArea) {
		//  Writes to the Null device always succeed (bit bucket)
		return onData (rArea.size ());
	    }
	};
	friend class BSPut;

    //  Construction
    public:
	VNullDeviceImplementation (VReferenceable *pContainer, ThisClass &rOther) : BaseClass (pContainer) {
	}
	VNullDeviceImplementation (VReferenceable *pContainer) : BaseClass (pContainer) {
	}

    //  Destruction
    public:
	~VNullDeviceImplementation () {
	}
    };
}


#endif
