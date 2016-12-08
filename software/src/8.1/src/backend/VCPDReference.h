#ifndef VCPDReference_Interface
#define VCPDReference_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "M_CPD.h"

/*************************
 *****  Definitions  *****
 *************************/

class VCPDReference : public VTransient {
//  Retention Control
protected:
    void retainReferent (unsigned int iIncrement) {
	if (m_pReferent)
	    m_pReferent->retain (iIncrement);
    }
    void retainReferent () {
	if (m_pReferent)
	    m_pReferent->retain ();
    }
    void releaseReferent () {
	if (m_pReferent)
	    m_pReferent->release ();
    }

//  Construction
public:
    VCPDReference (M_CPD *pRef, unsigned int xRef, RTYPE_Type xExpectedType = RTYPE_C_Any)
	: m_pReferent (pRef->GetCPD (xRef, xExpectedType))
    {
    }
    VCPDReference (M_CPD* pRef, int xRef, RTYPE_Type xExpectedType = RTYPE_C_Any)
	: m_pReferent (pRef->GetCPD (xRef, xExpectedType))
    {
    }
    VCPDReference (VCPDReference const& rOther) : m_pReferent (rOther.m_pReferent) {
	retainReferent ();
    }
    VCPDReference (unsigned int iIncrement, M_CPD* pOther) : m_pReferent (pOther) {
	retainReferent (iIncrement);
    }
    VCPDReference (M_CPD* pOther) : m_pReferent (pOther) {
	retainReferent ();
    }
    VCPDReference () : m_pReferent (0) {
    }

//  Destruction
public:
    ~VCPDReference () {
	releaseReferent ();
    }

//  Access
public:
    M_CPD& operator* () const {
	return *m_pReferent;
    }
    M_CPD* operator-> () const {
	return  m_pReferent;
    }
    operator M_CPD* () const {
	return  m_pReferent;
    }

//  Query
public:
    bool isEmpty () const {
	return 0 == m_pReferent;
    }
    bool isntEmpty () const {
	return 0 != m_pReferent;
    }

    bool isNil () const {
	return 0 == m_pReferent;
    }
    bool isntNil () const {
	return 0 != m_pReferent;
    }

//  Update
public:
    void claim (VCPDReference& rOther) {
	releaseReferent ();
	m_pReferent = rOther.m_pReferent;
	rOther.m_pReferent = 0;
    }
    void claim (M_CPD* pOther) {
	releaseReferent ();
	m_pReferent = pOther;
    }

    void setTo (M_CPD* pOther) {
	if (pOther != m_pReferent) {
	    if (pOther)
		pOther->retain ();

	    releaseReferent ();

	    m_pReferent = pOther;
	}
    }

    void clear () {
	releaseReferent ();
	m_pReferent = 0;
    }

    VCPDReference& operator= (VCPDReference const& rOther) {
	setTo (rOther);
	return *this;
    }
    VCPDReference& operator= (M_CPD* pOther) {
	setTo (pOther);
	return *this;
    }

//  State
protected:
    M_CPD* m_pReferent;
};

#endif
