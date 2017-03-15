#ifndef VOrdered_Interface
#define VOrdered_Interface

/************************
 *****  Components  *****
 ************************/

#include "VCollection.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "RTlink.h"

#include "VCollectionOfOrderables.h"

/*********************
 *****  Classes  *****
 *********************/

class VOrderedBase : public VCollection {
//  Construction
protected:
    VOrderedBase (VCollectionOfOrderables& rOrderableCollection);

    VOrderedBase (VOrderedBase& rOrderedCollection, rtLINK_CType* pRestriction);

//  Destruction
protected:
    ~VOrderedBase () {
    }

//  Access
public:
    M_CPD* ordering () const { return m_pOrdering; }

    void getOrdering (M_CPD*& rpOrdering) const {
	rpOrdering = m_pOrdering;
	if (rpOrdering)
	    rpOrdering->retain ();
    }

protected:
    M_CPD* restrictedOrdering (rtLINK_CType* pRestriction) const;

    unsigned int const* orderingArray () const { return m_pOrderingArray; }

//  Element Access
protected:
    void resetCursor (unsigned int xElement) {
	m_pOrderingElement = m_pOrderingArray + xElement;
    }

public:
    bool elementIsValid () const {
	return m_pOrderingElement < m_pOrderingLimit && m_iCachedElementIsValid;
    }

    unsigned int elementIndex () const {
	return m_pOrderingElement - m_pOrderingArray;
    }

//  State
protected:
    VCPDReference const		m_pOrdering;

    unsigned int const* const	m_pOrderingArray;
    unsigned int const*	const	m_pOrderingLimit;
    unsigned int const*		m_pOrderingElement;

    bool			m_iCachedElementIsValid;
};


/**********************
 *****  Template  *****
 **********************/

template <class CollectionClass, class ElementType> class VOrdered : public VOrderedBase {
//  Construction
public:
    VOrdered (CollectionClass& rOrderableCollection)
	: VOrderedBase (rOrderableCollection)
	, m_rOrderableCollection (rOrderableCollection)
    {
    }

    VOrdered (VOrdered<CollectionClass,ElementType>& rOrderedCollection, rtLINK_CType* pRestriction)
	: VOrderedBase (rOrderedCollection, pRestriction)
	, m_rOrderableCollection (rOrderedCollection.m_rOrderableCollection)
    {
    }

//  Destruction
public:
    ~VOrdered () {
    }

//  Access
public:
    CollectionClass& orderableCollection () const {
	return m_rOrderableCollection;
    }

//  Element Access
public:
    void resetCursor (unsigned int xElement = 0) {
	m_rOrderableCollection.refreshCache ();
	VOrderedBase::resetCursor (xElement);
	cacheElement ();
    }

    void goToNextElement () {
	if (m_pOrderingElement < m_pOrderingLimit)
	{
	    m_pOrderingElement++;
	    cacheElement ();
	}
    }

    void goToNextDifference ();

    void skip (unsigned int n) {
	m_pOrderingElement += n;
	if (m_pOrderingElement < m_pOrderingLimit)
	    cacheElement ();
	else
	    m_pOrderingElement = m_pOrderingLimit;
    }

    ElementType element () const {
	return m_iCachedElement;
    }

protected:
    void cacheElement ();

//  State
protected:
    CollectionClass&	m_rOrderableCollection;
    ElementType		m_iCachedElement;
};


/*********************
 *****  Members  *****
 *********************/

#if defined(_AIX) && defined(__TEMPINC__)
#pragma implementation("VOrdered.i")
#else
#include "VOrdered.i"
#endif


#endif
