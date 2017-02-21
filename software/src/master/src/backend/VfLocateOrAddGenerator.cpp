/*****  VfLocateOrAddGenerator Implementation  *****/

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

#include "VfLocateOrAddGenerator.h"

/************************
 *****  Supporting  *****
 ************************/


/**************************
 **************************
 *****  Construction  *****
 **************************
 **************************/

VfLocateOrAddGenerator::VfLocateOrAddGenerator (VAssociativeResult& rAssociativeResult)
: VAssociativeOperator		(rAssociativeResult)
, m_pTargetDomainAdjustment	(0)
, m_sTargetDomainAdjustment	(0)
{
}

/*************************
 *************************
 *****  Destruction  *****
 *************************
 *************************/

VfLocateOrAddGenerator::~VfLocateOrAddGenerator ()
{
    if (m_pTargetDomainAdjustment)
	m_pTargetDomainAdjustment->discard ();
}


/*****************
 *****************
 *****  Use  *****
 *****************
 *****************/

void VfLocateOrAddGenerator::initialize (
    M_CPD* pTargetPTokenRef, int xTargetPTokenRef, M_CPD* pSourcePToken
)
{
    VAssociativeOperator::initialize (
	pTargetPTokenRef, xTargetPTokenRef, pSourcePToken
    );

    if (m_pTargetDomainAdjustment)
    {
	m_pTargetDomainAdjustment->discard ();
	m_pTargetDomainAdjustment = NilOf (rtPTOKEN_CType*);
	m_sTargetDomainAdjustment = 0;
    }

    createSourceBoundTargetReference ();
}

void VfLocateOrAddGenerator::commit () {
    commitTargetReference (m_pTargetDomainAdjustment);
    commitSourceReference (m_sTargetDomainAdjustment);

    VAssociativeOperator::commit ();
}
