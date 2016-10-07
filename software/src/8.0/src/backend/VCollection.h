#ifndef VCollection_Interface
#define VCollection_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VCPDReference.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "venvir.h"

#include "RTptoken.h"

/*************************
 *****  Definitions  *****
 *************************/

class VCollection : public VTransient {
//  Construction
protected:
    VCollection (M_CPD* pDPT) : m_pDPT (pDPT) {
    }

//  Destruction
protected:
    ~VCollection () {
    }

//  Query
public:
    bool isScalar () const {
	return m_pDPT->NamesTheScalarPToken ();
    }

    unsigned int cardinality () const {
	return rtPTOKEN_CPD_BaseElementCount (m_pDPT);
    }
    M_CPD* ptoken () const {
	return m_pDPT;
    }

//  Exception Generation
protected:
    void raiseComponentTypeException (
	char const* pComponentName, M_CPD* pComponent
    ) const;

//  State
protected:
    VCPDReference m_pDPT;
};

#endif
