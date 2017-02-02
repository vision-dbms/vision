#ifndef VPrimaryCall_Interface
#define VPrimaryCall_Interface

/************************
 *****  Components  *****
 ************************/

#include "VCall.h"

#include "VSelector.h"

/**************************
 *****  Declarations  *****
 **************************/

class VByteCodeScanner;


/*************************
 *****  Definitions  *****
 *************************/

class VPrimaryCall : public VCall {
    DECLARE_CONCRETE_RTT (VPrimaryCall, VCall);

//  Meta Maker
protected:
    static void MetaMaker ();

//  Construction
public:
    VPrimaryCall (
	VTask *pCaller, rtLINK_CType *pCallerSubset, VByteCodeScanner const &rMessageSource
    );
    VPrimaryCall (
	VTask *pCaller, rtLINK_CType *pCallerSubset, char const *pMessageName, unsigned int iParameterCount
    );
    VPrimaryCall (
	VTask *pCaller, rtLINK_CType *pCallerSubset, unsigned int xMessageName
    );

//  Destruction
private:
    ~VPrimaryCall () {
    }

//  Access
public:
    VSelector const &selector_() const;

//  State
protected:
    VSelector const m_iSelector;
};


#endif
