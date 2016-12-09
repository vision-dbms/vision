#ifndef VkArray_Interface
#define VkArray_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    class V_API VkArray : public virtual VTransient {
	DECLARE_FAMILY_MEMBERS (VkArray, VTransient); 
    //  Aliases
    public:
	typedef unsigned int element_count_t;
	typedef unsigned int element_index_t;

    //  Construction
    protected:
	VkArray (element_count_t cElements = 0) : m_cElements (cElements) {
	}
	VkArray (ThisClass const &rOther) : m_cElements (rOther.m_cElements) {
	}

    //  Access
    public:
	element_count_t cardinality () const {
	    return m_cElements;
	}
	element_count_t elementCount () const {
	    return m_cElements;
	}

	bool isEmpty () const {
	    return m_cElements == 0;
	}
	bool isntEmpty () const {
	    return m_cElements != 0;
	}

    //  State
    protected:
	element_count_t m_cElements;
    };
}  // namespace V

#endif
