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
    VCollection (rtPTOKEN_Handle *pDPT) : m_pDPT (pDPT) {
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
	return m_pDPT->cardinality ();
    }
    rtPTOKEN_Handle *ptoken () const {
	return m_pDPT;
    }

//  Exception Generation
protected:
    void raiseComponentTypeException (
	char const *pComponentName, M_CPD *pComponent
    ) const {
	raiseComponentTypeException (pComponentName, pComponent->RType ());
    }
    void raiseComponentTypeException (
	char const *pComponentName, RTYPE_Type xComponentType
    ) const;

//  State
protected:
    rtPTOKEN_Handle::Reference m_pDPT;
};

#endif
