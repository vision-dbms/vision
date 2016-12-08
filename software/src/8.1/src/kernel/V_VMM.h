#ifndef V_VMM_Interface
#define V_VMM_Interface

/************************
 *****	Components  *****
 ************************/

#include "VReferenceable.h"

#ifdef __VMS

#ifndef __NEW_STARLET
#define __NEW_STARLET
#endif

#ifndef _LARGEFILE
#define _LARGEFILE
#endif

#include <gen64def.h>

#else
typedef char *__char_ptr64;

#endif

/**************************
 *****	Declarations  *****
 **************************/

class VkStatus;

typedef __char_ptr64     addr64_t;
typedef unsigned __int64 size64_t;

/*************************
 *****	Definitions  *****
 *************************/

namespace V {
    class V_API VMM : public VReferenceable {
	DECLARE_ABSTRACT_RTT (VMM, VReferenceable);

    //  Aliases
    public:
	typedef Reference MMR;

    //  Area
    public:
	class V_API Area {
	    DECLARE_FAMILY_MEMBERS (Area, void);

	//  Construction
	public:
	    Area (addr64_t pArea, size64_t sArea) : m_pArea (pArea), m_sArea (sArea) {
	    }
	    Area (Area const &rOther) : m_pArea (rOther.m_pArea), m_sArea (rOther.m_sArea) {
	    }
	    Area () : m_pArea (0), m_sArea (0) {
	    }

	//  Destruction
	public:
	    ~Area () {
	    }

	//  Access
	public:
	    addr64_t areaOrigin () const {
		return m_pArea;
	    }
	    addr64_t areaExtent () const {
		return m_pArea + m_sArea;
	    }
	    size64_t areaSize () const {
		return m_sArea;
	    }

	//  Query
	public:
	    bool contains (Area const &rOther) const {
		return areaOrigin () <= rOther.areaOrigin () && rOther.areaExtent () <= areaExtent ();
	    }

	    bool extendsOriginOf (Area const &rOther) const {
		return areaExtent () == rOther.areaOrigin ();
	    }
	    bool extendsExtentOf (Area const &rOther) const {
		return areaOrigin () == rOther.areaExtent ();
	    }
		
	    bool isNil () const {
		return m_pArea == 0 && m_sArea == 0;
	    }
	    bool isntNil () const {
		return m_pArea != 0 || m_sArea != 0;
	    }

	    bool operator <  (Area const &rOther) const {
		return areaExtent () <= rOther.areaOrigin (); // <= because areaExtent is 1 byte beyond the last byte in the area.
	    }
	    bool operator <= (Area const &rOther) const {
		return *this < rOther || *this == rOther;
	    }
	    bool operator == (Area const &rOther) const {
		return areaOrigin () == rOther.areaOrigin () && areaSize () == rOther.areaSize ();
	    }
	    bool operator != (Area const &rOther) const {
		return areaOrigin () != rOther.areaOrigin () || areaSize () != rOther.areaSize ();
	    }
	    bool operator >= (Area const &rOther) const {
		return *this > rOther || *this == rOther;
	    }
	    bool operator > (Area const &rOther) const {
		return areaOrigin () >= rOther.areaExtent (); // >= because areaExtent is 1 byte beyond the last byte in the area.
	    }

	//  Update
	public:
	    void clear () {
		m_pArea = 0;
		m_sArea = 0;
	    }

	    bool cover (Area const &rOther);

	    bool prune (Area const &rOther);
	    bool pruneOrigin (Area const &rOther);
	    bool pruneExtent (Area const &rOther);

	    void setTo (Area const &rOther) {
		m_pArea = rOther.m_pArea;
		m_sArea = rOther.m_sArea;
	    }
	    void setTo (addr64_t pArea, size64_t sArea) {
		m_pArea = pArea;
		m_sArea = sArea;
	    }

	    Area &operator= (Area const &rOther) {
		setTo (rOther);
		return *this;
	    }

	//  State
	private:
	    addr64_t	m_pArea;
	    size64_t	m_sArea;
	};

    //	Space
    public:
	class V_API Space {
	    DECLARE_FAMILY_MEMBERS (Space, void);

	//  Construction
	public:
	    Space ();

	//  Destruction
	public:
	    ~Space ();

	//  Access
	public:
#ifdef __VMS
	    unsigned int adjustedSectionFlags (unsigned int iInitialFlags) const;
	    bool getRegionID (VkStatus &rStatus, _generic_64 &rRegionID) const;
#endif

	    Area const &area () const {
		return m_iArea;
	    }
	    addr64_t areaOrigin () const {
		return m_iArea.areaOrigin ();
	    }
	    addr64_t areaExtent () const {
		return m_iArea.areaExtent ();
	    }
	    size64_t areaSize () const {
		return m_iArea.areaSize ();
	    }

	//  Management
	public:
	    bool acquireSpace (VkStatus &rStatus, VMM *pManager, size64_t sSpace);
	    bool acquireSpace (VkStatus &rStatus, addr64_t pSpace, size64_t sSpace) {
		Area const iSpace (pSpace, sSpace);
		return acquireSpace (rStatus, iSpace);
	    }
	    bool acquireSpace (VkStatus &rStatus, Area const &rSpace);
	    bool acquireSpace (VkStatus &rStatus, Space &rOther);
	    bool releaseSpace (VkStatus &rStatus);
	private:
	    bool releaseSpace ();

	//  State
	private:
	    Area	m_iArea;
	    MMR		m_pManager;
	};
	friend class Space;

    //	Construction
    protected:
	VMM ();

    //	Destruction
    protected:
	~VMM ();

    //  Access/Query
#ifdef __VMS
    protected:
	virtual unsigned int adjustedSectionFlags_(unsigned int iFlags, addr64_t pSpace) const = 0;
	virtual bool getRegionID_(VkStatus &rStatus, _generic_64 &rRegionID, addr64_t pSpace) const = 0;
    public:
	unsigned int adjustedSectionFlags (unsigned int iFlags, addr64_t pSpace) const {
	    return adjustedSectionFlags_(iFlags, pSpace);
	}
	bool getRegionID (VkStatus &rStatus, _generic_64 &rRegionID, addr64_t pSpace) const {
	    return getRegionID_(rStatus, rRegionID, pSpace);
	}
#endif

    //	Use
    protected:
	virtual bool acquireSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea, Area const &rSpace) = 0;
	virtual bool provideSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea, size64_t sSpace) = 0;
	virtual bool reclaimSpace_(VkStatus &rStatus, MMR &rpMM, Area &rArea) = 0;
    };
}

#endif
