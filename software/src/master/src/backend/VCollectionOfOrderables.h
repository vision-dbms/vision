#ifndef VCollectionOfOrderables_Interface
#define VCollectionOfOrderables_Interface

/************************
 *****  Components  *****
 ************************/

#include "VCollection.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class VCollectionOfOrderables : public VCollection {
//  Friends
    friend class VSorter;

//  Globals
protected:
    static unsigned int const g_pTrivialOrdering[1];

//  Construction
protected:
    VCollectionOfOrderables (M_CPD* pDPT) : VCollection (pDPT) {
    }

//  Destruction
protected:
    ~VCollectionOfOrderables () {
    }

//  Cache Maintenance
public:
    virtual void refreshCache ();

//  Ordering
public:
    M_CPD* orderDPT ();
    M_CPD* ordering ();

    unsigned int const* orderingArray ();

private:
    void createOrderingObjects ();

protected:
    virtual void sortOrderingArray (
	unsigned int *pOrderingArray, unsigned int sOrderingArray
    ) const;

//  Element Comparison
private:
    int compare (void const* pElement1, void const* pElement2) const {
	return compare (*(unsigned int const*)pElement1, *(unsigned int const*)pElement2);
    }

public:
    virtual int compare (unsigned int xElement1, unsigned int xElement2) const = 0;

//  State
protected:
    VCPDReference	m_pOrderDPT;
    VCPDReference	m_pOrdering;
    unsigned int const*	m_pOrderingArray;
};


#endif
