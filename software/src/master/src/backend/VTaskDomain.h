#ifndef VTaskDomain_Interface
#define VTaskDomain_Interface

/************************
 *****  Components  *****
 ************************/

#include "VInternalGroundStore.h"

#include "VCPDReference.h"
#include "VLinkCReference.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "venvir.h"

#include "RTptoken.h"


/*************************
 *****  Definitions  *****
 *************************/

class VTaskDomain : public VInternalGroundStore {
//  Run Time Type
    DECLARE_CONCRETE_RTT (VTaskDomain, VInternalGroundStore);

//  Path Type
public:
    enum PathType {
	PathType_Indeterminate,
	PathType_Identity,
	PathType_Parent,		// one-step, returned maps shared with 'this'
	PathType_Subset,	// derived 'link', release when done.
	PathType_Sequence,	// derived 'refuv', release when done.
	PathType_SequencedSubset// derived 'link' and 'refuv', release when done.
    };

//  Meta Maker
protected:
    static void MetaMaker ();

//  Globals
protected:
    static VReference<VTaskDomain> g_pGroundDomain;

//  Construction
protected:
    VTaskDomain (VTaskDomain* pParent, rtLINK_CType* pSubset, M_CPD* pReordering);
    VTaskDomain ();

public:
    VTaskDomain* childDomain (rtLINK_CType* pSubset, M_CPD* pReordering) {
	return pSubset || pReordering ? new VTaskDomain (this, pSubset, pReordering) : this;
    }
    static VTaskDomain* groundDomain ();

//  Query
public:
    bool isGround () const {
	return m_pParent.isNil ();
    }
    bool isScalar () const {
	return m_pDPT->NamesTheScalarPToken ();
    }
    bool isntScalar () const {
	return m_pDPT->DoesntNameTheScalarPToken ();
    }

//  Access
public:
    unsigned int cardinality () const {
	return m_pDPT->cardinality ();
    }
    virtual unsigned int cardinality_() const OVERRIDE {
	return cardinality ();
    }

    rtPTOKEN_Handle *ptoken () const {
	return m_pDPT;
    }
    virtual rtPTOKEN_Handle *ptoken_() const OVERRIDE {
	return ptoken ();
    }

    VTaskDomain* parent () const {
	return m_pParent;
    }
    rtLINK_CType* subset () const {
	return m_pSubset;
    }
    M_CPD* reordering() const {
	return m_pReordering;
    }

//  P-Token Creation
public:
    rtPTOKEN_Handle *NewPToken (unsigned int iCardinality) const {
	return new rtPTOKEN_Handle (m_pDPT->Space (), iCardinality);
    }
    rtPTOKEN_Handle *NewPToken () const {
	return NewPToken (m_pDPT->cardinality ());
    }

//  Path Access
protected:
    PathType getPathToAncestor (
	VTaskDomain* pDestination, rtLINK_CType*& rpRestriction, M_CPD*& rpReordering
    ) const;

public:
    PathType getPathMaps (
	VTaskDomain* pDestination, rtLINK_CType*& rpRestriction, M_CPD*& rpReordering
    ) const {
	PathType xReturnedPathType;
	if (this == pDestination) {
	    xReturnedPathType = PathType_Identity;
	    rpRestriction = 0;
	    rpReordering  = 0;
	}
	else if (pDestination == m_pParent) {
	    xReturnedPathType = PathType_Parent;
	    rpRestriction = m_pSubset;
	    rpReordering  = m_pReordering;
	}
	else xReturnedPathType = getPathToAncestor (
	    pDestination, rpRestriction, rpReordering
	);

	return xReturnedPathType;
    }

//  Path Cleanup
public:
    static void releasePathMaps (
	PathType xReturnedPathType, rtLINK_CType* pRestriction, M_CPD* pReordering
    );

//  State
protected:
    VReference<VTaskDomain> const	m_pParent;
    VLinkCReference const		m_pSubset;
    VCPDReference const			m_pReordering;
    rtPTOKEN_Handle::Reference const	m_pDPT;
};


#endif
