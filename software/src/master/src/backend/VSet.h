#ifndef VSet_Interface
#define VSet_Interface

/************************
 *****  Components  *****
 ************************/

#include "VTransient.h"

#include "VCPDReference.h"

/**************************
 *****  Declarations  *****
 **************************/

#include "VAssociativeResult.h"

/*********************
 *****  Classes  *****
 *********************/

class VSet : public VTransient {
//  Construction
protected:
    VSet () {
    }

//  Destruction
protected:
    ~VSet () {
    }

//  Deferred Initialization
protected:
    virtual void initializeDPT () = 0;

//  Alignment
public:
    virtual void align () = 0;

//  Access
public:
    M_CPD* ptoken () {
	if (m_pDPT.isNil ())
	    initializeDPT ();
	return m_pDPT;
    }

//  Update
public:	// ...should be protected
    void adjustDomain (unsigned int xDomainOrigin, int sDomainAdjustment);

    void installDPT (M_CPD* pDPT) {
	m_pDPT.setTo (pDPT);
    }

//  State
protected:
    VCPDReference m_pDPT;
};


#endif
