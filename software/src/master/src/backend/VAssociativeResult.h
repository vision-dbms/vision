#ifndef VAssociativeResult_Interface
#define VAssociativeResult_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "m.h"

#include "RTlink.h"
#include "RTptoken.h"

#include "VOrdered.h"


/*********************
 *****  Classes  *****
 *********************/

/************************
 *  VAssociativeResult  *
 ************************/

class VAssociativeResult : public VTransient {
//  Friends
    friend class VAssociativeOperator;
    friend class VfLocateOrAddGenerator;
    friend class VfLocateGenerator;
    friend class VfDeleteGenerator;

//  Construction
public:
    VAssociativeResult (bool fScalarModeEnabled = false);

//  Destruction
public:
    ~VAssociativeResult ();

//  Initialization
protected:
    void assertScalaricity (unsigned int iCardinality) const {
	if (iCardinality > 1)
	    raiseScalaricityException (iCardinality);
    }

    void initialize (rtPTOKEN_Handle *pTargetPToken, rtPTOKEN_Handle *pSourcePToken);

    void createTargetBoundTargetReference () {
	if (!m_fIsScalar)
	    m_pTargetReference = rtLINK_RefConstructor (m_pTargetPToken);
    }

    void createSourceBoundTargetReference () {
	if (!m_fIsScalar)
	    m_pTargetReference = rtLINK_PosConstructor (m_pSourcePToken);
    }

    void createSourceReference () {
	if (!m_fIsScalar)
	    m_pSourceReference = rtLINK_RefConstructor (m_pSourcePToken);
    }

    void createTargetTrajectory (rtPTOKEN_CType *&rpTrajectory) {
	rpTrajectory = m_pTargetPToken->makeAdjustments ();
    }

    void appendTargetRepeat (unsigned int xElementOrigin, unsigned int sRepetition) {
	if (m_fIsScalar) {
	    assertScalaricity (sRepetition);
	    m_xTargetReference = xElementOrigin;
	    m_fTargetReferenceValid = true;
	}
	else rtLINK_AppendRepeat (m_pTargetReference, xElementOrigin, sRepetition);
    }
    void appendSourceRange (unsigned int xRangeOrigin, unsigned int sRange) {
	if (m_fIsScalar) {
	    assertScalaricity (sRange);
	    m_xSourceReference = xRangeOrigin;
	    m_fSourceReferenceValid = true;
	}
	else rtLINK_AppendRange (m_pSourceReference, xRangeOrigin, sRange);
    }

    void commitTargetReference () {
	if (m_pTargetReference)
	    m_pTargetReference->Close (m_pSourcePToken);
    }
    void commitTargetReference (rtPTOKEN_CType*& rpTrajectory);
    void commitTargetReference (unsigned int sReferenceDomain);
    void commitSourceReference (unsigned int sReferenceDomain);

    void commit () { m_fCommitted = true; }

//  Access
public:
    bool isValid () const { return m_fCommitted; }
    bool isScalar () const { return m_fIsScalar; }

    bool targetReferenceIsValid () const {
	return IsntNil (m_pTargetReference) || m_fTargetReferenceValid;
    }
    bool sourceReferenceIsValid () const {
	return IsntNil (m_pSourceReference) || m_fSourceReferenceValid;
    }

    unsigned int targetCardinality () const {
    	return m_pTargetPToken->cardinality ();
    }
    unsigned int sourceCardinality () const {
	return m_pSourcePToken->cardinality ();
    }

    rtPTOKEN_Handle *targetPToken () const { return m_pTargetPToken; }
    rtPTOKEN_Handle *sourcePToken () const { return m_pSourcePToken; }

    rtLINK_CType* targetReference () const { return m_pTargetReference; }
    rtLINK_CType* sourceReference () const { return m_pSourceReference; }

    unsigned int targetReferenceScalar () const { return m_xTargetReference; }
    unsigned int sourceReferenceScalar () const { return m_xSourceReference; }

    rtLINK_CType* claimedTargetReference () {
	rtLINK_CType* pResult = m_pTargetReference;
	m_pTargetReference = NilOf (rtLINK_CType*);
	return pResult;
    }
    rtLINK_CType* claimedSourceReference () {
	rtLINK_CType* pResult = m_pSourceReference;
	m_pSourceReference = NilOf (rtLINK_CType*);
	return pResult;
    }

//  Exception Generation
protected:
    void raiseScalaricityException (unsigned int iCardinality) const;

//  State
protected:
    rtPTOKEN_Handle::Reference m_pTargetPToken;
    rtPTOKEN_Handle::Reference m_pSourcePToken;
    rtLINK_CType*	m_pTargetReference;
    rtLINK_CType*	m_pSourceReference;
    unsigned int const	m_fScalarModeEnabled	: 1;
    unsigned int	m_fIsScalar		: 1;
    unsigned int	m_fCommitted		: 1;
    unsigned int	m_fTargetReferenceValid	: 1;
    unsigned int	m_fSourceReferenceValid	: 1;
    unsigned int	m_xTargetReference;
    unsigned int	m_xSourceReference;
};


/**************************
 *  VAssociativeOperator  *
 **************************/

class VAssociativeOperator : public VTransient {
//  Construction
protected:
    VAssociativeOperator (VAssociativeResult& rResult) : m_rResult (rResult) {
    }

//  Destruction
protected:
    ~VAssociativeOperator () {
    }

//  Access
public:
    bool inScalarMode () const { return m_rResult.isScalar (); }

    unsigned int targetCardinality () const { return m_rResult.targetCardinality (); }
    unsigned int sourceCardinality () const { return m_rResult.sourceCardinality (); }

    rtPTOKEN_Handle *targetPToken () const { return m_rResult.targetPToken (); }
    rtPTOKEN_Handle *sourcePToken () const { return m_rResult.sourcePToken (); }

    rtLINK_CType* targetReference () const { return m_rResult.targetReference (); }
    rtLINK_CType* sourceReference () const { return m_rResult.sourceReference (); }

//  Use
public:
    virtual void initialize (rtPTOKEN_Handle *pTargetPToken, rtPTOKEN_Handle *pSourcePToken) = 0;

protected:
    void createTargetBoundTargetReference () {
	m_rResult.createTargetBoundTargetReference ();
    }
    void createSourceBoundTargetReference () {
	m_rResult.createSourceBoundTargetReference ();
    }
    void createSourceReference () {
	m_rResult.createSourceReference ();
    }

    void appendTargetRepeat (unsigned int xElementOrigin, unsigned int sRepetition) {
	m_rResult.appendTargetRepeat (xElementOrigin, sRepetition);
    }
    void appendSourceRange (unsigned int xRangeOrigin, unsigned int sRange) {
	m_rResult.appendSourceRange (xRangeOrigin, sRange);
    }

    void createTargetTrajectory (rtPTOKEN_CType*& rpTrajectory) {
	m_rResult.createTargetTrajectory (rpTrajectory);
    }

    void commitTargetReference () {
	m_rResult.commitTargetReference ();
    }
    void commitTargetReference (rtPTOKEN_CType*& rpTrajectory) {
	m_rResult.commitTargetReference (rpTrajectory);
    }
    void commitTargetReference (unsigned int sReferenceDomain) {
	m_rResult.commitTargetReference (sReferenceDomain);
    }
    void commitSourceReference (unsigned int sReferenceDomain) {
	m_rResult.commitSourceReference (sReferenceDomain);
    }

    void commit () {
	m_rResult.commit ();
    }

//  State
protected:
    VAssociativeResult& m_rResult;
};



#endif
