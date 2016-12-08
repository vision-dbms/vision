#ifndef VCachedArray_Interface
#define VCachedArray_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

// #define VCachedArray_CacheImplemented

template <class ElementType, unsigned int CacheSize> class VCachedArray : public VTransient {
//  Construction
public:
    VCachedArray (unsigned int iCount)
	: m_iCount (iCount)
#if defined(VCachedArray_CacheImplemented)
	, m_pArray (iCount > CacheSize ? new ElementType[iCount - CacheSize] : 0)
#else
	, m_pArray (iCount > 1 ? new ElementType[iCount] : iCount > 0 ? new ElementType : 0)
#endif
    {
    }

//  Destruction
public:
    ~VCachedArray () {
#if defined(VCachedArray_CacheImplemented)
	if (m_pArray)
	    delete[] m_pArray;
#else
	switch (m_iCount) {
	case 0:
	    break;
	case 1:
	    delete m_pArray;
	    break;
	default:
	    delete[] m_pArray;
	    break;
	}
#endif
    }

//  Access
public:
    unsigned int count () const {
	return m_iCount;
    }

    ElementType& operator[] (unsigned int xElement) {
#if defined(VCachedArray_CacheImplemented)
	return xElement < CacheSize
	    ? m_iArray[xElement]
	    : m_pArray[xElement - CacheSize];
#else
	return m_pArray[xElement];
#endif
    }

    ElementType const& operator[] (unsigned int xElement) const {
#if defined(VCachedArray_CacheImplemented)
	return xElement < CacheSize
	    ? m_iArray[xElement]
	    : m_pArray[xElement - CacheSize];
#else
	return m_pArray[xElement];
#endif
    }

//  State
protected:
    unsigned int const	m_iCount;
#if defined(VCachedArray_CacheImplemented)
    ElementType		m_iArray[CacheSize];
#endif
    ElementType* const	m_pArray;
};


#endif
