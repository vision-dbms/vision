/*****  Distribution Generator Interface  *****/
#ifndef VfDistributionGenerator_Interface
#define VfDistributionGenerator_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

class M_CPD;

#include "VkRadixListSorter.h"


/******************************
 *****  Class Definition  *****
 ******************************/

class VfDistributionGenerator {
/*****  Construction/Initialization  *****/
public:
    VfDistributionGenerator (M_CPD*& pDistributionCPD);

    void initializeUsingComainPToken (M_CPD* pPTokenCPD);
    void initializeUsingDomainPToken (M_CPD* pPTokenCPD);

private:
    void initializeDistribution ();

/*****  Destruction/Cleanup  *****/
public:
    ~VfDistributionGenerator ();

private:
    void releaseContent ();

/*****  Update  *****/
public:
    void emitMonotone (
	unsigned int xDomainRegion, unsigned int sDomainRegion
    ) {
	unsigned int* pDomainRegion = m_pDistributionArray + xDomainRegion;
	while (sDomainRegion-- > 0)
	    *pDomainRegion++ = xDomainRegion++;
    }

    void emitSort (
	unsigned int	    xDomainRegion,
	unsigned int	    sDomainRegion,
	unsigned int const *pSourceRegion
    ) {
	unsigned int* pDomainRegion = m_pDistributionArray + xDomainRegion;
	UTIL_RadixListSort (pSourceRegion, pDomainRegion, (size_t)sDomainRegion);
	while (sDomainRegion-- > 0)
	    *pDomainRegion++ += xDomainRegion;
    }

/*****  Access  *****/
public:
    M_CPD* distributionComainPTokenCPD () const {
	return m_pDistributionComainPToken;
    }

    M_CPD* distributionDomainPTokenCPD () const {
	return m_pDistributionDomainPToken;
    }

    M_CPD* distributionCPD () const {
	return m_pDistributionCPD;
    }

    unsigned int at (unsigned int xDomainElement) {
	return m_pDistributionArray [xDomainElement];
    }

/*****  State  *****/
protected:
    M_CPD*		m_pDistributionComainPToken;
    M_CPD*		m_pDistributionDomainPToken;
    M_CPD*&		m_pDistributionCPD;
    unsigned int*	m_pDistributionArray;
};


#endif
