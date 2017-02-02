#ifndef VkMapOf_Interface
#define VkMapOf_Interface

/************************
 *****  Components  *****
 ************************/

#include "VkSetOf.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

template <class KLT,class KVT,class KKT,class ELT> class VkMapOf : public VkSet {
//  Aliases
public:
    typedef VkMapOf<KLT,KVT,KKT,ELT> Map;

    typedef KLT key_storage_t;
    typedef KVT key_insert_t;
    typedef KKT key_lookup_t;
    typedef ELT value_t;

public:
    class AssociationType : public VTransient {
    //  Construction
    public:
	AssociationType (AssociationType const &rOther)
	    : m_iKey (rOther.m_iKey), m_iValue (rOther.m_iValue)
	{
	}

	AssociationType () {
	}

    //  Destruction
    public:
	~AssociationType () {
	}

    //  Access
    public:
	KLT const &key () const {
	    return m_iKey;
	}
	ELT const &value () const {
	    return m_iValue;
	}
	ELT &value () {
	    return m_iValue;
	}

    //  Query
    public:
	bool operator <  (KKT iKeyValue) const {
	    return m_iKey <  iKeyValue;
	}
	bool operator <= (KKT iKeyValue) const {
	    return m_iKey <= iKeyValue;
	}
	bool operator == (KKT iKeyValue) const {
	    return m_iKey == iKeyValue;
	}
	bool operator != (KKT iKeyValue) const {
	    return m_iKey != iKeyValue;
	}
	bool operator >= (KKT iKeyValue) const {
	    return m_iKey >= iKeyValue;
	}
	bool operator >  (KKT iKeyValue) const {
	    return m_iKey >  iKeyValue;
	}

    //  Initialization
    public:
	AssociationType &operator= (AssociationType const &rOther) {
	    m_iKey = rOther.m_iKey;
	    m_iValue = rOther.m_iValue;
	    return *this;
	}
	AssociationType &operator= (KVT iKeyValue) {
	    m_iKey = iKeyValue;
	    return *this;
	}

    //  State
    protected:
	KLT m_iKey;
	ELT m_iValue;
    };
    typedef VkSetOf<AssociationType,KVT,KKT> Set;
    typedef typename Set::Iterator SetIterator;
    
public:
    class Iterator : public SetIterator {
    //  Construction
    public:
	Iterator (Map &rMap) : SetIterator (rMap.m_iAssociations), m_rMap (rMap) {
	}

    //  Content
    public:
	KLT const &key () const {
	    return m_rMap.key (m_xElement);
	}
	ELT &value () const {
	    return m_rMap[m_xElement];
	}
	operator ELT& () const {
	    return value ();
	}

    //  State
    protected:
	USING (Set::Iterator::m_xElement)
	Map &m_rMap;
    };

//  Friends
    friend class Iterator;

//  Construction
public:
    VkMapOf () {
    }

//  Destruction
public:
    ~VkMapOf () {
    }

//  Access
public:
    ELT &operator[] (unsigned int xElement) const {
	return const_cast<AssociationType&>(m_iAssociations[xElement]).value ();
    }

    AssociationType const &association (unsigned int xElement) const {
	return m_iAssociations[xElement];
    }
    KLT const &key (unsigned int xElement) const {
	return m_iAssociations[xElement].key ();
    }
    ELT &value (unsigned int xElement) const { 
	return (*this)[xElement];
    }
    unsigned int cardinality () const {
	return m_iAssociations.cardinality ();
    }

    bool Contains (KKT iKey) const {
	unsigned int xElement;
	return m_iAssociations.Locate (iKey, xElement);
    }

    bool isEmpty () const {
	return m_iAssociations.isEmpty ();
    }
    bool isntEmpty () const {
	return m_iAssociations.isntEmpty ();
    }

    bool Locate (KKT iKey, unsigned int &rxElement) const {
	return m_iAssociations.Locate (iKey, rxElement);
    }

    template <class destination_t> void SendTo (destination_t pDestination) {
	if (pDestination) {
	    Iterator pElement (*this);
	    while (pElement.isntAtEnd ()) {
		(*pElement++).value()->sendTo (pDestination);
	    }
	}
    }

//  Update
public:
    bool Insert (KVT iKey, unsigned int &rxElement) {
	return m_iAssociations.Insert (iKey, rxElement);
    }
    bool Delete (KKT iKey) {
	return m_iAssociations.Delete (iKey);
    }
    void DeleteAll () {
	m_iAssociations.DeleteAll ();
    }

//  State
protected:
    Set m_iAssociations;
};


#endif
