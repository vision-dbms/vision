/*****  Distribution Generator Implementation  *****/

/*********************************
 *****  Imported Interfaces  *****
 *********************************/

/*****  System  *****/
#include "Vk.h"

/*****  Self  *****/
#include "VfDistributionGenerator.h"

/*****  Supporting  *****/
#include "M_CPD.h"

#include "RTptoken.h"
#include "RTrefuv.h"


/*******************************************
 *****  Construction / Initialization  *****
 *******************************************/

VfDistributionGenerator::VfDistributionGenerator (
    M_CPD*& pDistributionCPD
) : m_pDistributionCPD (pDistributionCPD)
{
    m_pDistributionCPD		=
    m_pDistributionComainPToken	=
    m_pDistributionDomainPToken	= NilOf (M_CPD*);
}

void VfDistributionGenerator::initializeUsingComainPToken (M_CPD* pPTokenCPD) {
    releaseContent ();

    m_pDistributionComainPToken = rtPTOKEN_BasePToken (pPTokenCPD, -1);
    m_pDistributionDomainPToken = rtPTOKEN_New (
	M_AttachedNetwork->ScratchPad (),
	rtPTOKEN_CPD_BaseElementCount (m_pDistributionComainPToken)
    );

    initializeDistribution ();
}

void VfDistributionGenerator::initializeUsingDomainPToken (M_CPD* pPTokenCPD) {
    releaseContent ();

    m_pDistributionDomainPToken = rtPTOKEN_BasePToken (pPTokenCPD, -1);
    m_pDistributionComainPToken = rtPTOKEN_New (
	M_AttachedNetwork->ScratchPad (),
	rtPTOKEN_CPD_BaseElementCount (m_pDistributionDomainPToken)
    );

    initializeDistribution ();
}

void VfDistributionGenerator::initializeDistribution () {
    if (m_pDistributionCPD)
	m_pDistributionCPD->release ();

    m_pDistributionCPD = rtREFUV_New (
	m_pDistributionDomainPToken, m_pDistributionComainPToken
    );
    m_pDistributionArray = (unsigned int*)rtREFUV_CPD_Array (m_pDistributionCPD);
}


/***********************************
 *****  Destruction / Cleanup  *****
 ***********************************/

VfDistributionGenerator::~VfDistributionGenerator ()
{
    releaseContent ();
}

void VfDistributionGenerator::releaseContent () {
    if (m_pDistributionComainPToken)
	m_pDistributionComainPToken->release ();
    if (m_pDistributionDomainPToken)
	m_pDistributionDomainPToken->release ();
}
