#ifndef VArrayOf_Interface
#define VArrayOf_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "m.h"

#include "RTptoken.h"

/*************************
 *****  Definitions  *****
 *************************/

/*------------------*
 *----  VArray  ----*
 *------------------*/

class VArray : public VTransient {
//  Construction
protected:
    VArray (rtPTOKEN_Handle *pDPT) : m_iElementCount (pDPT->cardinality ()) {
    }
    VArray (unsigned int iElementCount = 0) : m_iElementCount (iElementCount) {
    }

//  Access
public:
    unsigned int cardinality () const {
	return m_iElementCount;
    }
    unsigned int elementCount () const {
	return m_iElementCount;
    }

//  P-Token Manufacture
public:
    rtPTOKEN_Handle *newBasePToken () const {
	return new rtPTOKEN_Handle (M_AttachedNetwork->ScratchPad (), m_iElementCount);
    }

//  State
protected:
    unsigned int m_iElementCount;
};

template <class ElementType> class VArrayOf : public VArray {
//  Element Array Management
protected:
    ElementType* newElementArray (unsigned int iElementCount) const {
	return iElementCount > 1
	    ? new ElementType[iElementCount]
	    : iElementCount > 0
	    ? new ElementType
	    : 0;
    }
    ElementType* newElementArray () const {
	return newElementArray (m_iElementCount);
    }

    void deleteElementArray () const {
	switch (m_iElementCount) {
	case 0:
	    break;
	case 1:
	    delete m_pElementArray;
	    break;
	default:
	    delete[] m_pElementArray;
	    break;
	}
    }

//  Construction
public:
    VArrayOf (rtPTOKEN_Handle *pDPT) : VArray (pDPT), m_pElementArray (newElementArray ()) {
    }
    VArrayOf (
	unsigned int iElementCount = 0
    ) : VArray (iElementCount), m_pElementArray (newElementArray ()) {
    }

//  Destruction
public:
    ~VArrayOf () {
	deleteElementArray ();
    }

//  Access
public:
    ElementType const* elementArray () const {
	return m_pElementArray;
    }

    ElementType& element (unsigned int xElement) const {
	return m_pElementArray[xElement];
    }
    ElementType& operator[] (unsigned int xElement) const {
	return m_pElementArray[xElement];
    }

//  Element Insertion and Deletion
public:
    rtPTOKEN_Handle *editUsingDPTChain (rtPTOKEN_Handle *pDPT);

    void insert (unsigned int xInsertionOrigin, unsigned int iElementCount);

    void append (unsigned int iElementCount) {
	insert (m_iElementCount, iElementCount);
    }
    void prepend (unsigned int iElementCount) {
	insert (0, iElementCount);
    }

//  State
protected:
    ElementType* m_pElementArray;
};


/*********************
 *****  Members  *****
 *********************/

#if defined(_AIX) && defined(__TEMPINC__)
#pragma implementation("VArrayOf.i")
#else
#include "VArrayOf.i"
#endif


#endif
