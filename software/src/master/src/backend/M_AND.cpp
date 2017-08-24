/*****  M_AND Implementation  *****/

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

#include "M_AND.h"

/************************
 *****  Supporting  *****
 ************************/

#include "verrdef.h"

#include "M_CPD.h"
#include "VDatabaseFederatorForBatchvision.h"

#if defined(_WIN32)
#pragma hdrstop
#endif


/***************************
 ***************************
 *****  Run Time Type  *****
 ***************************
 ***************************/

DEFINE_CONCRETE_RTT (M_AND);

/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

M_AND::M_AND (
    VDatabaseFederatorForBatchvision*	pDatabaseFederator,
    VDatabaseNDF*			pDatabaseNDF,
    char const*				pVersionSpec,
    bool				makingNewNDF
)
: VDatabaseActivation	(pDatabaseFederator)
, m_pPhysicalAND	(PS_AND::AccessNetwork (pDatabaseNDF, pVersionSpec, makingNewNDF))
, m_cActiveSpaces	(m_pPhysicalAND->SpaceCount ())
, m_pGCMetrics		(0)
, m_pASDRing		(0)
{
//  Initialize the 'Nil' POP, ...
    M_POP_D_ObjectSpace	   (m_iNilPOP) = M_KnownSpace_Nil;
    M_POP_D_ContainerIndex (m_iNilPOP) = M_KnownCTI_SpaceRoot;

//  ... the ASD vector, ...
    for (unsigned int i = 0; i < M_POP_MaxObjectSpace; i++)
	m_iASDVector [i] = NilOf (M_ASD*);

//  ... and the scratch pad:
    new M_ASD (this, 0);
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

M_AND::~M_AND () {
    if (m_pGCMetrics)
	delete m_pGCMetrics;
}


/********************
 ********************
 *****  Access  *****
 ********************
 ********************/

VDatabase *M_AND::database_() const {
    return m_pPhysicalAND->database ();
}

bool M_AND::ContainerExists (M_POP const *pReference) {
    M_ASD* pASD = AccessSpace (pReference);
    unsigned int xContainer = M_POP_ContainerIndex (pReference);

    return IsntNil (pASD)
	&& xContainer < pASD->cteCount ()
	&& pASD->cte (xContainer)->isReferenced ();
}

bool M_AND::ContainerPersists (M_POP const *pReference) {
    PS_CTE iCTE;
    return (M_POP_ObjectSpace (pReference) < SpaceCount ()) &&
	m_pPhysicalAND->AccessSpace(M_POP_ObjectSpace (pReference))->GetLiveCTE (
	    M_POP_ContainerIndex (pReference), iCTE
    );
}

VContainerHandle *M_AND::SafeGetContainerHandle (M_POP const *pReference) {
    return ContainerExists (pReference) ? GetContainerHandle (pReference) : 0;
}

VContainerHandle *M_AND::SafeGetContainerHandle (M_POP const *pReference, RTYPE_Type xType) {
    VContainerHandle *pContainerHandle = SafeGetContainerHandle (pReference);
    if (pContainerHandle && pContainerHandle->RTypeIsnt (xType)) {
	pContainerHandle->release ();
	pContainerHandle = 0;
    }
    return pContainerHandle;
}

/*************************
 *************************
 *****  Maintenance  *****
 *************************
 *************************/

bool M_AND::align (M_POP const *pReference) {
    VContainerHandle::Reference pHandle (SafeGetContainerHandle (pReference));
    return pHandle && pHandle->align ();
}

bool M_AND::alignAll (M_POP const *pReference, bool bCleaning) {
    VContainerHandle::Reference pHandle (SafeGetContainerHandle (pReference));
    return pHandle && pHandle->alignAll (bCleaning);
}

/*****************************
 *****************************
 *****  Name Resolution  *****
 *****************************
 *****************************/

bool M_AND::LocateName (M_ASD *pThat, unsigned int xThatName, M_TOP &rName) {
//  Special case NIL...
    if (xThatName == M_KnownCTI_SpaceRoot && pThat->Index () == M_KnownSpace_Nil) {
	rName.setTo (this, M_KnownSpace_Nil, M_KnownCTI_SpaceRoot);
	return true;
    }

//  ... otherwise, look for the space:
    for (unsigned int xSpace = 0; xSpace < m_cActiveSpaces; xSpace++) {
	if (pThat == m_iASDVector[xSpace]) {
	    rName.setTo (this, xSpace, xThatName);
	    return true;
	}
    }

    return false;
}

bool M_AND::CreateName (M_ASD *pThat, unsigned int xThatName, M_TOP &rName) {
    if (m_cActiveSpaces > M_POP_MaxObjectSpace) raiseException (
	EC__UsageError, "M_AND::CreateName: Too Many Object Spaces"
    );
    else {
	m_iASDVector[m_cActiveSpaces] = pThat;
	rName.setTo (this, m_cActiveSpaces++, xThatName);
    }

    return false;
}
