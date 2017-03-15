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
    M_CPD *&pDistributionCPD
) : m_pDistributionCPD (pDistributionCPD) {
    m_pDistributionCPD = NilOf (M_CPD*);
}

void VfDistributionGenerator::initializeUsingComainPToken (rtPTOKEN_Handle *pPToken) {
    releaseContent ();

    m_pDistributionComainPToken.setTo (pPToken->basePToken ());
    m_pDistributionDomainPToken.setTo (
	new rtPTOKEN_Handle (
	    M_AttachedNetwork->ScratchPad (), m_pDistributionComainPToken->cardinality ()
	)
    );

    initializeDistribution ();
}

void VfDistributionGenerator::initializeUsingDomainPToken (rtPTOKEN_Handle *pPToken) {
    releaseContent ();

    m_pDistributionDomainPToken.setTo (pPToken->basePToken ());
    m_pDistributionComainPToken.setTo (
	new rtPTOKEN_Handle (
	    M_AttachedNetwork->ScratchPad (), m_pDistributionDomainPToken->cardinality ()
	)
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
    m_pDistributionComainPToken.clear ();
    m_pDistributionDomainPToken.clear ();
}
