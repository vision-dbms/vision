#ifndef VkSetOf_Interface
#define VkSetOf_Interface

/************************
 *****  Components  *****
 ************************/

#include "VkSet.h"
#include "VkDynamicArrayOf.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

template <class ELType, class EVType, class EKType = EVType> class VkSetOf : public VkSet {
public:
    typedef ELType ELT;
    typedef EVType EVT;
    typedef EKType EKT;

    typedef VkSetOf<ELType,EVType,EKType> Set;
    DECLARE_FAMILY_MEMBERS (Set, VkSet);

// Iterator
public:
    class Iterator {
    // Construction
    public:
	Iterator (Set &rSet) : m_rSet (rSet), m_xElement (0) {
	}

	Iterator (Iterator const &rOther)
	    : m_rSet (rOther.m_rSet), m_xElement (rOther.m_xElement)
	{
	}

    // Query
    public:
	bool operator!= (Iterator const &rOther) const {
	    return m_xElement != rOther.m_xElement || (&m_rSet != &(rOther.m_rSet));
	}

	void setToBegin () {
	    m_xElement = 0;
	}

	void setToEnd () {
	    m_xElement = m_rSet.cardinality ();
	}

	// Is this right?
	bool isAtBegin () const {
	    return m_xElement == 0;
	}

	bool isAtEnd () const {
	    return m_xElement == m_rSet.cardinality ();
	}

	bool isntAtEnd () const {
	    return m_xElement != m_rSet.cardinality ();
	}

	bool isNotAtEnd () const {
	    return isntAtEnd ();
	}

	Iterator begin () const {
	    Iterator T (*this);
	    return T;
	}

	Iterator end () const {
	    Iterator T (*this);
	    T.setToEnd ();
	    return T;
	}

	unsigned int index () const {
	    return m_xElement;
	}

	Iterator& operator++ () {
	    if (m_xElement < m_rSet.cardinality ())
    		m_xElement++;
	    return *this;
	}

	Iterator operator++ (int) {
	    if (m_xElement < m_rSet.cardinality ()) {
		Iterator T (*this);
		++ (*this);
		return T;
	    }
	    return *this;
	}

	ELType const & operator* () const {
	    return m_rSet[m_xElement];
	}
	
	bool Delete () {
	    return m_rSet.Delete (m_xElement);
	}

    // State
    protected:
	Set &m_rSet;
	unsigned int m_xElement;
    };

//  Construction
public:
    VkSetOf () {
    }

//  Destruction
public:
    ~VkSetOf () {
    }

//  Query
protected:
    int Compare (unsigned int xElement, EKType iKey) const {
	ELType const &rElement = m_iElementArray[xElement];
	return rElement < iKey ? -1 : rElement == iKey ? 0 : 1;
    }

public:
    bool Contains (EKType iKey) const {
	unsigned int xElement;
	return Locate (iKey, xElement);
    }

    bool isEmpty () const {
	return m_iElementArray.isEmpty ();
    }
    bool isntEmpty () const {
	return m_iElementArray.isntEmpty ();
    }

//  Access
public:
    ELType const &operator[] (unsigned int xElement) const {
	return m_iElementArray[xElement];
    }

    unsigned int cardinality () const {
	return m_iElementArray.cardinality ();
    }

    bool Locate (EKType iKey, unsigned int &rxElement) const {
	bool found = false;
	int ll = -1;
	int ul = cardinality ();
	while (ul - ll > 1) {
	    int index = (ul + ll) / 2;
	    int iComparisonResult = Compare (index, iKey);
	    if (iComparisonResult == 0) {
		found = true;
		ul = index;
		break;
	    }
	    else if (iComparisonResult < 0)
		ll = index;
	    else
		ul = index;
	}

	rxElement = ul;

	return found;
    }

    template <class destination_t> void SendTo (destination_t pDestination) {
	if (pDestination) {
	    Iterator pElement (*this);
	    while (pElement.isntAtEnd ()) {
		(*pElement++)->sendTo (pDestination);
	    }
	}
    }

//  Update
public:
    /*---------------------------------------------------------------------
     *  The scalar 'Insert' and 'Delete' routines return true if the set was
     *  updated and false if no changes were made.
     *---------------------------------------------------------------------
     */
    bool Insert (EVType iKey, unsigned int &rxElement) {
	if (Locate (iKey, rxElement))
	    return false;

	m_iElementArray.Insert (rxElement, 1);
	m_iElementArray[rxElement] = iKey;
	return true;
    }
    bool Insert (EVType iKey) {
	unsigned int xElement;
	return Insert (iKey, xElement);
    }

    bool Delete (EKType iKey) {
	unsigned int xElement;
	if (!Locate (iKey, xElement))
	    return false;

	m_iElementArray.Delete (xElement);
	return true;
    }
    bool Delete (unsigned int xElement) {
	if (xElement < cardinality ()) {
	    m_iElementArray.Delete (xElement);
	    return true;
	}
	return false;
    }

    void DeleteAll () {
        m_iElementArray.DeleteAll();
    }

//  State
protected:
    VkDynamicArrayOf<ELType> m_iElementArray;
};


#endif
