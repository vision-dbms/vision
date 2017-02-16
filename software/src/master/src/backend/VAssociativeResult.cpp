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

VAssociativeResult::~VAssociativeResult () {
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
    rtPTOKEN_Handle *pTargetPToken, rtPTOKEN_Handle *pSourcePToken
) {
    m_pTargetPToken.setTo (pTargetPToken->basePToken ());
    m_pSourcePToken.setTo (pSourcePToken->basePToken ());
    if (m_pTargetReference) {
	m_pTargetReference->release ();
	m_pTargetReference = 0;
    }
    if (m_pSourceReference) {
	m_pSourceReference->release ();
	m_pSourceReference = 0;
    }

    m_fIsScalar = m_fScalarModeEnabled && sourceCardinality () == 1;
    m_fCommitted = m_fTargetReferenceValid = m_fSourceReferenceValid = false;
}

void VAssociativeResult::commitTargetReference (rtPTOKEN_CType*& rpTrajectory) {
    //  Close the trajectory if it's open, ...
    if (rpTrajectory) {
	m_pTargetPToken.setTo (rpTrajectory->ToPToken ());
	rpTrajectory = NilOf (rtPTOKEN_CType*);
    }

    //  ... and close the target reference if it exists:
    if (m_pTargetReference)
	m_pTargetReference->Close (m_pTargetPToken);
}

void VAssociativeResult::commitTargetReference (unsigned int sReferenceDomain) {
    if (m_pTargetReference || m_pSourceReference) {
	rtPTOKEN_Handle::Reference pReferenceDomainPToken (
	    new rtPTOKEN_Handle (M_AttachedNetwork->ScratchPad (), sReferenceDomain)
	);
	if (m_pTargetReference)
	    m_pTargetReference->Close (pReferenceDomainPToken);
	if (m_pSourceReference)
	    m_pSourceReference->Close (pReferenceDomainPToken);
    }
}

void VAssociativeResult::commitSourceReference (unsigned int sReferenceDomain) {
    if (m_pSourceReference) {
	m_pSourceReference->Close (
	    new rtPTOKEN_Handle (M_AttachedNetwork->ScratchPad (), sReferenceDomain)
	);
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
    rtPTOKEN_Handle *pTargetPToken, rtPTOKEN_Handle *pSourcePToken
) {
    m_rResult.initialize (pTargetPToken, pSourcePToken);
}
