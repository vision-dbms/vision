#ifndef Vca_VFDIODeviceFactory_Interface
#define Vca_VFDIODeviceFactory_Interface

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
    class Vca_API VFDIODeviceImplementation : public VDeviceImplementation {
	DECLARE_FAMILY_MEMBERS (VFDIODeviceImplementation, VDeviceImplementation);

    //  Device Reference
    public:
	typedef VReference<ThisClass> DeviceReference;

    //  Factory Data
    public:
	typedef ThisClass Data;

    //  Handle
    public:
	typedef int Handle;

    //  Use
    public:
	class Use : public VDeviceImplementation::AbstractUse {
	    DECLARE_FAMILY_MEMBERS (Use, VDeviceImplementation::AbstractUse);

	//  Friends
	    friend class VFDIODeviceImplementation;

	//  Reference
	public:
	    typedef VReference<ThisClass> Reference;

	//  Construction
	protected:
	    Use (
		VReferenceable *pContainer, VFDIODeviceImplementation *pDevice
	    ) : BaseClass (pContainer), m_pDevice (pDevice) {
	    }

	//  Destruction
	protected:
	    ~Use () {
	    }

	//  Access
	public:
	    VFDIODeviceImplementation *device () const {
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

	    friend class VFDIODeviceImplementation;

	//  Area
	public:
	    typedef VDeviceBSReadArea Area;

	//  Construction
	protected:
	    BSGet (
		VReferenceable *pContainer, VFDIODeviceImplementation *pDevice
	    ) : BaseClass (pContainer, pDevice) {
	    }

	//  Destruction
	protected:
	    ~BSGet () {
	    }

	//  Access
	public:
	    Area const &area () const {
		return m_iArea;
	    }
	    void *areaBase () const {
		return m_iArea.base ();
	    }
	    size_t areaSize () const {
		return m_iArea.size ();
	    }

	//  Startup
	public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	    bool start (VkStatus &rStatus, Area const &rArea);

	//  State
	private:
	    Area m_iArea;
	};
	friend class BSGet;

    //  BSPut
    public:
	class BSPut : public Use {
	    DECLARE_FAMILY_MEMBERS (BSPut, Use);

	    friend class VFDIODeviceImplementation;

	//  Area
	public:
	    typedef VDeviceBSWriteArea Area;

	//  Construction
	protected:
	    BSPut (
		VReferenceable *pContainer, VFDIODeviceImplementation *pDevice
	    ) : BaseClass (pContainer, pDevice) {
	    }

	//  Destruction
	protected:
	    ~BSPut () {
	    }

	//  Access
	public:
	    Area const &area () const {
		return m_iArea;
	    }
	    void const *areaBase () const {
		return m_iArea.base ();
	    }
	    size_t areaSize () const {
		return m_iArea.size ();
	    }

	//  Startup
	public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	    bool start (VkStatus &rStatus, Area const &rArea);

	//  State
	protected:
	    Area m_iArea;
	};
	friend class BSPut;

    //  Construction
    private:
	static bool canGet (int xOpenMode);
	static bool canPut (int xOpenMode);
    public:
	VFDIODeviceImplementation (VReferenceable *pContainer, VFDIODeviceImplementation &rOther);
	VFDIODeviceImplementation (VReferenceable *pContainer, Handle &rhDevice, int xOpenMode);

    //  Destruction
    public:
	~VFDIODeviceImplementation ();

    //  Access
    public:
	operator Handle () const {
	    return m_hDevice;
	}
	Handle grab () {
	    return grab (m_hDevice);
	}
	static Handle grab (Handle &rhDevice) {
	    Handle hDevice = rhDevice;
	    rhDevice = -1;
	    return hDevice;
	}

    //  Device Use
    protected:
	bool doTransfer (VkStatus &rStatus, ssize_t &rsTransfer, BSGet::Area const &rArea);
	bool doTransfer (VkStatus &rStatus, ssize_t &rsTransfer, BSPut::Area const &rArea);

    //  User Accounting
    private:
	void onEndOfFlow ();
    public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	void onFinalReader ();
	void onFinalWriter ();

    //  State
    private:
	Handle	m_hDevice;
	bool	m_bNoMoreGets;
	bool	m_bNoMorePuts;
    };
}


#endif
