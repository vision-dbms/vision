#ifndef VkArrayOf_Interface
#define VkArrayOf_Interface

/************************
 *****  Components  *****
 ************************/

#include "VkArray.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

namespace V {
    template <class ElementStorage_T> class VkArrayOf : public VkArray {
	DECLARE_FAMILY_MEMBERS (VkArrayOf<ElementStorage_T>, VkArray);

    //  Aliases
    public:
	typedef ElementStorage_T element_storage_t;
	typedef typename element_storage_t::element_t element_t;

	typedef element_t EType;

    //  Construction
    protected:
	VkArrayOf (element_count_t cElements) : BaseClass (cElements), m_iStorage (cElements) {
	}
	VkArrayOf (ThisClass const &rOther) : BaseClass (rOther), m_iStorage (elementCount (), rOther.m_iStorage) {
	}

    //  Destruction
    protected:
	~VkArrayOf () {
	}

    //  Access
    public:
	element_t const *elementArray () const {
	    return m_iStorage.elementArray ();
	}
	element_t const *element (element_index_t xElement) const {
	    return m_iStorage.element (elementCount (), xElement);
	}
	element_t const &operator[] (element_index_t xElement) const {
	    return *element (xElement);
	}

	element_t *elementArray () {
	    return m_iStorage.elementArray ();
	}
	element_t *element (element_index_t xElement) {
	    return m_iStorage.element (elementCount (), xElement);
	}
	element_t &operator[] (element_index_t xElement) {
	    return *element (xElement);
	}

    //  State
    protected:
	element_storage_t m_iStorage;
    };
} // namespace V

#endif
