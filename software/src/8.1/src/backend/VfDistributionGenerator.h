/*****  Distribution Generator Interface  *****/
#ifndef VfDistributionGenerator_Interface
#define VfDistributionGenerator_Interface

/************************
 *****  Components  *****
 ************************/

/**************************
 *****  Declarations  *****
 **************************/

#include "RTptoken.h"

#include "VkRadixListSorter.h"


/******************************
 *****  Class Definition  *****
 ******************************/

class VfDistributionGenerator {
/*****  Construction/Initialization  *****/
public:
    VfDistributionGenerator (M_CPD*& pDistributionCPD);

    void initializeUsingComainPToken (rtPTOKEN_Handle *pPToken);
    void initializeUsingDomainPToken (rtPTOKEN_Handle *pPToken);

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
    rtPTOKEN_Handle *distributionComainPToken () const {
	return m_pDistributionComainPToken;
    }

    rtPTOKEN_Handle *distributionDomainPToken () const {
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
    rtPTOKEN_Handle::Reference	m_pDistributionComainPToken;
    rtPTOKEN_Handle::Reference	m_pDistributionDomainPToken;
    M_CPD*&			m_pDistributionCPD;
    unsigned int*		m_pDistributionArray;
};


#endif
