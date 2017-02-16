/*****  VfLocateOrAddGenerator Interface  *****/
#ifndef VfLocateOrAddGenerator_Interface
#define VfLocateOrAddGenerator_Interface

/************************
 *****  Components  *****
 ************************/

#include "VAssociativeResult.h"

/**************************
 *****  Declarations  *****
 **************************/


/*******************
 *****  Class  *****
 *******************/

class VfLocateOrAddGenerator : public VAssociativeOperator {
//  Construction
public:
    VfLocateOrAddGenerator (VAssociativeResult& rResult);

//  Destruction
public:
    ~VfLocateOrAddGenerator ();

//  Access
public:
    int additionsGenerated () const {
	return m_rResult.sourceReferenceIsValid ();
    }

//  Use
public:
    void initialize (
	M_CPD* pTargetPTokenRef, int xTargetPTokenRef, M_CPD* pSourcePToken
    );
    inline void emitReference (
	unsigned int xSourceDomainRegion,
	unsigned int sSourceDomainRegion,
	unsigned int xTargetDomainElement,
	bool	     fTargetDomainElementMustBeAdded
    );
    inline void emitReference (
	unsigned int xSourceDomainRegion, unsigned int sSourceDomainRegion
    );
    void commit ();

/*****  State  *****/
protected:
    rtPTOKEN_CType*	m_pTargetDomainAdjustment;
    unsigned int	m_sTargetDomainAdjustment;
};


/*********************
 *****  Members  *****
 *********************/

inline void VfLocateOrAddGenerator::emitReference (
    unsigned int xSourceDomainRegion,
    unsigned int sSourceDomainRegion,
    unsigned int xTargetDomainElement,
    bool	 fTargetDomainElementMustBeAdded
)
{
    xTargetDomainElement += m_sTargetDomainAdjustment;
    appendTargetRepeat (xTargetDomainElement, sSourceDomainRegion);
    if (!fTargetDomainElementMustBeAdded)
	return;

    if (IsNil (m_pTargetDomainAdjustment))
    {
	createSourceReference ();
	createTargetTrajectory (m_pTargetDomainAdjustment);
    }
    appendSourceRange (xSourceDomainRegion, 1);
    m_pTargetDomainAdjustment->AppendAdjustment (xTargetDomainElement, 1);
    m_sTargetDomainAdjustment++;
}

inline void VfLocateOrAddGenerator::emitReference (
    unsigned int xSourceDomainRegion, unsigned int sSourceDomainRegion
) {
    emitReference (
	xSourceDomainRegion, sSourceDomainRegion, targetCardinality (), false
    );
}


#endif
