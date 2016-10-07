#ifndef VUtilityCall_Interface
#define VUtilityCall_Interface

/************************
 *****  Components  *****
 ************************/

#include "VCall.h"

/**************************
 *****  Declarations  *****
 **************************/

/*************************
 *****  Definitions  *****
 *************************/

class ABSTRACT VUtilityCall : public VCall {
//  Run Time Type
    DECLARE_ABSTRACT_RTT (VUtilityCall, VCall);

//  Meta Maker
protected:
    static void MetaMaker ();

//  Construction
protected:
    VUtilityCall (
	VTask* pCaller, rtLINK_CType* pCallerSubset, Type xType, unsigned int iParameterCount
    ) : VCall (pCaller, pCallerSubset, xType, iParameterCount) {
    }

//  Access
public:
    VSelector const& selector_ () const;

//  State
protected:
};


#endif
