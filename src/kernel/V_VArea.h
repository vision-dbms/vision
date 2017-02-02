#ifndef V_VArea_Interface
#define V_VArea_Interface

/*********************
 *****  Library  *****
 *********************/

#include "V.h"

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    template<typename P> class VArea {
    //  Aliases
    public:
	typedef VArea<P> ThisClass;
	typedef P Pointer;

    //  Construction/Destruction
    public:
	VArea (
	    P pRegion, size_t sRegion
	) : m_pRegion (pRegion), m_sRegion (sRegion) {
	}
	VArea (
	    ThisClass const &rOther
	) : m_pRegion (rOther.m_pRegion), m_sRegion (rOther.m_sRegion) {
	}
	VArea () : m_pRegion (0), m_sRegion (0) {
	}
	~VArea () {
	}

    //  Access
    public:
	P base () const {
	    return m_pRegion;
	}
	size_t size () const {
	    return m_sRegion;
	}

    //  Update
    public:
	ThisClass &operator= (ThisClass const &rOther) {
	    setTo (rOther);
	    return *this;
	}
	void setTo (ThisClass const &rOther) {
	    setTo (rOther.m_pRegion, rOther.m_sRegion);
	}
	void setTo (P pRegion, size_t sRegion) {
	    m_pRegion = pRegion;
	    m_sRegion = sRegion;
	}

    //  State
    private:
	P	m_pRegion;
	size_t	m_sRegion;
    };
}


#endif
