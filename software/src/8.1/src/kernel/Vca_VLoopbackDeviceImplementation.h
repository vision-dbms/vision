#ifndef Vca_VLoopbackDeviceImplementation_Interface
#define Vca_VLoopbackDeviceImplementation_Interface

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
    class Vca_API VLoopbackDeviceImplementation : public VDeviceImplementation {
	DECLARE_FAMILY_MEMBERS (VLoopbackDeviceImplementation, VDeviceImplementation);

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
	    friend class VLoopbackDeviceImplementation;

	//  Reference
	public:
	    typedef VReference<ThisClass> Reference;

	//  Construction
	protected:
	    Use (
		VReferenceable *pContainer, VLoopbackDeviceImplementation *pDevice
	    ) : BaseClass (pContainer), m_pDevice (pDevice) {
	    }

	//  Destruction
	protected:
	    ~Use () {
	    }

	//  Access
	public:
	    VLoopbackDeviceImplementation *device () const {
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

	    friend class VLoopbackDeviceImplementation;

	//  Area
	public:
	    typedef VDeviceBSReadArea Area;

	//  Construction
	protected:
	    BSGet (
		VReferenceable *pContainer, VLoopbackDeviceImplementation *pDevice
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
	    bool start (VkStatus &rStatus, Area const &rArea) {
		m_iArea.setTo (rArea);
		return m_pDevice->doRead (rStatus, this);
	    }

	//  State
	private:
	    Area m_iArea;
	};
	friend class BSGet;

    //  BSPut
    public:
	class BSPut : public Use {
	    DECLARE_FAMILY_MEMBERS (BSPut, Use);

	    friend class VLoopbackDeviceImplementation;

	//  Area
	public:
	    typedef VDeviceBSWriteArea Area;

	//  Construction
	protected:
	    BSPut (
		VReferenceable *pContainer, VLoopbackDeviceImplementation *pDevice
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
	    bool start (VkStatus &rStatus, Area const &rArea) {
		m_iArea.setTo (rArea);
		return m_pDevice->doWrite (rStatus, this);
	    }

	//  State
	protected:
	    Area m_iArea;
	};
	friend class BSPut;

    //  Construction
    public:
	VLoopbackDeviceImplementation (VReferenceable *pContainer, VLoopbackDeviceImplementation &rOther)
	    : BaseClass (pContainer)
	    , m_pPendingGet (rOther.m_pPendingGet)
	    , m_pPendingPut (rOther.m_pPendingPut)
	    , m_bNoMoreGets (rOther.m_bNoMoreGets)
	    , m_bNoMorePuts (rOther.m_bNoMorePuts)
	{
	}
	VLoopbackDeviceImplementation (VReferenceable *pContainer)
	    : BaseClass (pContainer)
	    , m_bNoMoreGets (false)
	    , m_bNoMorePuts (false)
	{
	}

    //  Destruction
    public:
	~VLoopbackDeviceImplementation () {
	}

    //  Device Use
    private:
	static void doTransfer (BSGet *pGet, BSPut *pPut);
    protected:
	bool doRead  (VkStatus &rStatus, BSGet *pGet);
	bool doWrite (VkStatus &rStatus, BSPut *pPut);

    //  User Accounting
    public:	/*>>>> 'public:' == MSVC6 bug workaround!!!  <<<<*/
	void onFinalReader ();
	void onFinalWriter ();

    //  State
    private:
	BSGet::Pointer	m_pPendingGet;
	BSPut::Pointer	m_pPendingPut;
	bool		m_bNoMoreGets;
	bool		m_bNoMorePuts;
    };
}


#endif
