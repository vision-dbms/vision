#ifndef Vos_VMSMemoryRegion_Interface
#define Vos_VMSMemoryRegion_Interface

/************************
 *****	Components  *****
 ************************/

#include "V_VMM.h"

/**************************
 *****	Declarations  *****
 **************************/

/*************************
 *****	Definitions  *****
 *************************/

namespace V {
    namespace OS {
#ifdef __VMS
	class VMSMemoryRegion : public VMM {
	    DECLARE_CONCRETE_RTT (VMSMemoryRegion, VMM);

	//  Handle
	    class Handle {
		DECLARE_FAMILY_MEMBERS (Handle, void);

	    //	Construction
	    public:
		Handle (VkStatus &rConstructionStatus, size64_t sRegion);
		Handle (Handle &rOther);
	    private:
		bool ownership () {
		    bool bOwner = m_bOwner;
		    m_bOwner = false;
		    return bOwner;
		}

	    //	Destruction
	    public:
		~Handle ();

	    //	Access
	    public:
		_generic_64 const &regionID () const {
		    return m_hRegion;
		}
		size64_t regionSize () const {
		    return m_iRegion.areaSize ();
		}

	    //  Query
	    public:
		bool isValid () const {
		    return m_iRegion.isntNil ();
		}
		bool isOwner () const {
		    return m_bOwner;
		}

	    //	Use
	    public:
		bool acquireSpace (VkStatus &rStatus, Area &rArea, Area const &rSpace);
		bool provideSpace (VkStatus &rStatus, Area &rArea, size64_t sSpace);
		bool reclaimSpace (VkStatus &rStatus, Area &rArea);
	    public:
		bool expandRegion (
		    VkStatus &rStatus, addr64_t &rpAllocation, size64_t &rsAllocation, size64_t sAllocation
		);

	    //	State
	    private:
		_generic_64	m_hRegion;
		Area		m_iRegion;
		bool		m_bOwner;	//  must be last.
	    };

	//  Construction
	public:
	    static ThisClass *New (size64_t sRegion);
	private:
	    VMSMemoryRegion (Handle &rHandle);

	//  Destruction
	private:
	    ~VMSMemoryRegion ();

	//  Access
	private:
	    virtual unsigned int adjustedSectionFlags_(unsigned int iFlags, addr64_t pSpace) const OVERRIDE;
	    virtual bool getRegionID_(VkStatus &rStatus, _generic_64 &rRegionID, addr64_t pSpace) const OVERRIDE;
	public:
	    _generic_64 const &regionID () const {
		return m_iHandle.regionID ();
	    }

	//  Query
	public:
	    bool isValid () const {
		return m_iHandle.isValid ();
	    }

	//  Use
	private:
	    virtual bool acquireSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea, Area const &rSpace) OVERRIDE;
	    virtual bool provideSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea, size64_t sSpace ) OVERRIDE;
	    virtual bool reclaimSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea) OVERRIDE;
	public:
	    bool expandRegion (
		VkStatus &rStatus, addr64_t &rpAllocation, size64_t &rsAllocation, size64_t sAllocation
	    );

	private:
	    Handle m_iHandle;
	};
#endif //__VMS
    }
}

#endif
