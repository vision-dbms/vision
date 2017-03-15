/*****  VfLocateGenerator Interface  *****/
#ifndef VfLocateGenerator_Interface
#define VfLocateGenerator_Interface

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

class VfLocateGenerator : public VAssociativeOperator {
//  Construction
public:
    VfLocateGenerator (VAssociativeResult& rResult);

//  Destruction
public:
    ~VfLocateGenerator () {
    }

//  Use
public:
    void initialize (rtPTOKEN_Handle *pTargetPToken, rtPTOKEN_Handle *pSourcePToken);
    inline void emitReference (
	unsigned int xSourceDomainRegion,
	unsigned int sSourceDomainRegion,
	unsigned int xTargetDomainElement,
	bool	     xTargetDomainElementNotFound
    );
    void commit ();

private:
    void createGuard ();

    void createGuardIfNecessary () {
	if (IsNil (sourceReference ()))
	    createGuard ();
    }

//  State
protected:
    unsigned int m_sGuardDomain;
};


/*********************
 *****  Members  *****
 *********************/

inline void VfLocateGenerator::emitReference (
    unsigned int xSourceDomainRegion,
    unsigned int sSourceDomainRegion,
    unsigned int xTargetDomainElement,
    bool	 xTargetDomainElementNotFound
)
{
    if (!xTargetDomainElementNotFound)
    {
	if (xSourceDomainRegion > m_sGuardDomain)
	{
	    createGuardIfNecessary ();
	    appendSourceRange (xSourceDomainRegion, sSourceDomainRegion);
	}
	appendTargetRepeat (xTargetDomainElement, sSourceDomainRegion);
	m_sGuardDomain += sSourceDomainRegion;
    }
}


#endif
