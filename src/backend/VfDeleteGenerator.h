#ifndef VfDeleteGenerator_Interface
#define VfDeleteGenerator_Interface

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

class VfDeleteGenerator : public VAssociativeOperator {
//  Construction
public:
    VfDeleteGenerator (VAssociativeResult& rResult);

//  Destruction
public:
    ~VfDeleteGenerator ();

//  Use
public:
    void initialize (rtPTOKEN_Handle *pTargetPToken, rtPTOKEN_Handle *pSourcePToken);
    inline void emitDeletion (
	unsigned int xSourceDomainElement, unsigned int xTargetDomainElement
    );
    void commit ();

//  State
protected:
    rtPTOKEN_CType*	m_pTargetDomainAdjustment;
    unsigned int	m_sTargetDomainAdjustment;
};


/*********************
 *****  Members  *****
 *********************/

inline void VfDeleteGenerator::emitDeletion (
    unsigned int xSourceDomainElement, unsigned int xTargetDomainElement
)
{
    if (IsNil (m_pTargetDomainAdjustment))
	createTargetTrajectory (m_pTargetDomainAdjustment);

    m_pTargetDomainAdjustment->AppendAdjustment (
	xTargetDomainElement + 1 - m_sTargetDomainAdjustment, -1
    );
    appendSourceRange (xSourceDomainElement, 1);

    m_sTargetDomainAdjustment++;
}


#endif
