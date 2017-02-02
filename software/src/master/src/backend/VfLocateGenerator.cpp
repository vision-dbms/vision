/*****  VfLocateGenerator Implementation  *****/

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

#include "VfLocateGenerator.h"

/************************
 *****  Supporting  *****
 ************************/


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VfLocateGenerator::VfLocateGenerator (VAssociativeResult& rResult)
: VAssociativeOperator	(rResult)
, m_sGuardDomain	(0)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/


/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void VfLocateGenerator::initialize (
    M_CPD* pTargetPTokenRef, int xTargetPTokenRef, M_CPD* pSourcePToken
)
{
    VAssociativeOperator::initialize (
	pTargetPTokenRef, xTargetPTokenRef, pSourcePToken
    );

    m_sGuardDomain = 0;

    createTargetBoundTargetReference ();
}

void VfLocateGenerator::createGuard () {
    createSourceReference ();
    appendSourceRange (0, m_sGuardDomain);
}

void VfLocateGenerator::commit () {
    if (sourceCardinality () > m_sGuardDomain)
    {
	createGuardIfNecessary ();
	commitTargetReference (m_sGuardDomain);
    }
    else commitTargetReference ();

    VAssociativeOperator::commit ();
}
