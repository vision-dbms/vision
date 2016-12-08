#ifndef VCollectionOf_Interface
#define VCollectionOf_Interface

/************************
 *****  Components  *****
 ************************/

#include "VCollectionOfOrderables.h"

/**************************
 *****  Declarations  *****
 **************************/

class VAssociativeResult;

/*************************
 *****  Definitions  *****
 *************************/

template <class ElementType> class VCollectionOf : public VCollectionOfOrderables {
//  Construction
public:
    VCollectionOf (rtPTOKEN_Handle *pDPT, ElementType const* pElementArray)
	: VCollectionOfOrderables (pDPT), m_pElementArray (pElementArray)
    {
    }


//  Destruction
public:
    ~VCollectionOf () {
    }

//  Element Comparison
public:
    int compare (unsigned int xElement1, unsigned int xElement2) const;

//  Element Access
public:
    bool GetElement (unsigned int xElement, ElementType& rElement) {
	rElement = m_pElementArray[xElement];
	return true;
    }

//  State
protected:
    ElementType const* const m_pElementArray;
};


/*********************
 *****  Members  *****
 *********************/

#if defined(_AIX) && defined(__TEMPINC__)
#pragma implementation("VCollectionOf.i")
#else
#include "VCollectionOf.i"
#endif


/****************************
 *****  Instantiations  *****
 ****************************/

class VCollectionOfUnsigned32 : public VCollectionOf<unsigned int> {
//  Aliases
public:
    typedef Vdd::Store Store;

//  Construction
public:
    VCollectionOfUnsigned32 (rtPTOKEN_Handle *pDPT, unsigned int const* pElementArray)
	: VCollectionOf<unsigned int> (pDPT, pElementArray)
    {
    }

//  Ordering
protected:
    void sortOrderingArray (
	unsigned int *pOrderingArray, unsigned int sOrderingArray
    ) const;

//  Use
public:
    void insertInto (
	Store *pSet, M_CPD*& rpReordering, VAssociativeResult& rAssociativeResult
    );
    void locateIn (
	Store *pSet, M_CPD*& rpReordering, VAssociativeResult& rAssociativeResult
    );
    void deleteFrom (
	Store *pSet, M_CPD*& rpReordering, VAssociativeResult& rAssociativeResult
    );
};


#endif
