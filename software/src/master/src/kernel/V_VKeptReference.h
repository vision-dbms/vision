#ifndef Vxa_VKeptReference_Interface
#define Vxa_VKeptReference_Interface

/************************
 *****  Components  *****
 ************************/

#include "VReferenceable.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    template <typename T> class VKeptReference {
	DECLARE_NUCLEAR_FAMILY(VKeptReference<T>);

    //  Aliases
    public:
	typedef T kept_t;
	typedef ::VReferenceable keeper_t;
	typedef keeper_t::Reference keeper_reference_t;

    //  Construction
    public:
	VKeptReference (kept_t &rKept, keeper_t *pKeeper = 0) : m_rKept (rKept), m_pKeeper (pKeeper) {
	}

    //  Destruction
    public:
	~VKeptReference () {
	}

    //  Access
    public:
	operator kept_t& () const {
	    return m_rKept;
	}

    //  State
    private:
	kept_t &m_rKept;
	keeper_reference_t const m_pKeeper;
    };
}


#endif
