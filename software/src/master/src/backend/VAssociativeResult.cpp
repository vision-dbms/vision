/*****  VAssociativeResult Implementation  *****/

/************************
 ************************
 *****  Interfaces  *****
 ************************
 ************************/

/********************
 *****  System  *****
 ********************/

#include "Vk.h"

/******************
 *****  Self  *****
 ******************/

#include "VAssociativeResult.h"

/************************
 *****  Supporting  *****
 ************************/

#include "verrdef.h"


/********************************
 *****                      *****
 *****  VAssociativeResult  *****
 *****                      *****
 ********************************/

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VAssociativeResult::VAssociativeResult (bool fScalarModeEnabled)
: m_fScalarModeEnabled	(fScalarModeEnabled)
, m_fCommitted		(false)
, m_pTargetReference	(0)
, m_pSourceReference	(0)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VAssociativeResult::~VAssociativeResult ()
{
    if (m_pTargetReference)
	m_pTargetReference->release ();
    if (m_pSourceReference)
	m_pSourceReference->release ();
}


/****************************
 ****************************
 *****  Initialization  *****
 ****************************
 ****************************/

void VAssociativeResult::initialize (
    M_CPD* pTargetPTokenRef, int xTargetPTokenRef, M_CPD* pSourcePToken
)
{
    m_pTargetPToken.claim (rtPTOKEN_BasePToken (pTargetPTokenRef, xTargetPTokenRef));
    m_pSourcePToken.claim (rtPTOKEN_BasePToken (pSourcePToken, -1));
    if (m_pTargetReference)
    {
	m_pTargetReference->release ();
	m_pTargetReference = 0;
    }
    if (m_pSourceReference)
    {
	m_pSourceReference->release ();
	m_pSourceReference = 0;
    }

    m_fIsScalar = m_fScalarModeEnabled && sourceCardinality () == 1;
    m_fCommitted = m_fTargetReferenceValid = m_fSourceReferenceValid = false;
}

void VAssociativeResult::commitTargetReference (rtPTOKEN_CType*& rpTrajectory) {
    //  Close the trajectory if it's open, ...
    if (rpTrajectory) {
	m_pTargetPToken.claim (rpTrajectory->ToPToken ());
	rpTrajectory = NilOf (rtPTOKEN_CType*);
    }

    //  ... and close the target reference if it exists:
    if (m_pTargetReference)
	m_pTargetReference->Close (m_pTargetPToken);
}

void VAssociativeResult::commitTargetReference (unsigned int sReferenceDomain)
{
    if (m_pTargetReference || m_pSourceReference)
    {
	M_CPD* pReferenceDomainPToken = rtPTOKEN_New (
	    M_AttachedNetwork->ScratchPad (), sReferenceDomain
	);
	if (m_pTargetReference)
	    m_pTargetReference->Close (pReferenceDomainPToken);
	if (m_pSourceReference)
	    m_pSourceReference->Close (pReferenceDomainPToken);
	pReferenceDomainPToken->release ();
    }
}

void VAssociativeResult::commitSourceReference (unsigned int sReferenceDomain) {
    if (m_pSourceReference) {
	M_CPD* pReferenceDomainPToken = rtPTOKEN_New (
	    M_AttachedNetwork->ScratchPad (), sReferenceDomain
	);
	m_pSourceReference->Close (pReferenceDomainPToken);
	pReferenceDomainPToken->release ();
    }
}

void VAssociativeResult::raiseScalaricityException (unsigned int iCardinality) const {
    raiseException (
	EC__UsageError,
	"VAssociativeResult: Cardinality of %u violates scalaricity precondition.",
	iCardinality
    );
}


/**********************************
 ****                         *****
 *****  VAssociativeOperator  *****
 *****                        *****
 **********************************/

void VAssociativeOperator::initialize (
    M_CPD* pTargetPTokenRef, int xTargetPTokenRef, M_CPD* pSourcePToken
) {
    m_rResult.initialize (pTargetPTokenRef, xTargetPTokenRef, pSourcePToken);
}
